/**
*** Copyright (c) 2016-2019, Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp.
*** Copyright (c) 2020-2021, Jaguar0625, gimre, BloodyRookie.
*** Copyright (c) 2022-present, Kriptxor Corp, Microsula S.A.
*** All rights reserved.
***
*** This file is part of BitxorCore.
***
*** BitxorCore is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** BitxorCore is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with BitxorCore. If not, see <http://www.gnu.org/licenses/>.
**/

#include "NotificationPublisher.h"
#include "Block.h"
#include "BlockUtils.h"
#include "FeeUtils.h"
#include "NotificationSubscriber.h"
#include "TransactionPlugin.h"

namespace bitxorcore { namespace model {

	namespace {
		void RequireKnown(EntityType entityType) {
			if (BasicEntityType::Other == ToBasicEntityType(entityType))
				BITXORCORE_THROW_RUNTIME_ERROR_1("NotificationPublisher only supports Block and Transaction entities", entityType);
		}

		BlockNotification CreateBlockNotification(const Block& block, const Address& blockSignerAddress) {
			return { block.Type, blockSignerAddress, block.BeneficiaryAddress, block.Timestamp, block.Difficulty, block.FeeMultiplier };
		}

		class CustomNotificationPublisher : public NotificationPublisher {
		public:
			explicit CustomNotificationPublisher(const TransactionRegistry& transactionRegistry)
					: m_transactionRegistry(transactionRegistry)
			{}

		public:
			void publish(const WeakEntityInfoT<VerifiableEntity>& entityInfo, NotificationSubscriber& sub) const override {
				RequireKnown(entityInfo.type());

				if (BasicEntityType::Transaction != ToBasicEntityType(entityInfo.type()))
					return;

				auto height = entityInfo.isAssociatedBlockHeaderSet() ? entityInfo.associatedBlockHeader().Height : Height(0);
				return publish(static_cast<const Transaction&>(entityInfo.entity()), entityInfo.hash(), height, sub);
			}

			void publish(const Transaction& transaction, const Hash256& hash, Height height, NotificationSubscriber& sub) const {
				const auto& plugin = *m_transactionRegistry.findPlugin(transaction.Type);

				PublishContext context;
				context.SignerAddress = GetSignerAddress(transaction);
				context.BlockHeight = height;
				plugin.publish(WeakEntityInfoT<Transaction>(transaction, hash), context, sub);
			}

		private:
			const TransactionRegistry& m_transactionRegistry;
		};

		class BasicNotificationPublisher : public NotificationPublisher {
		public:
			BasicNotificationPublisher(
					const TransactionRegistry& transactionRegistry,
					UnresolvedTokenId feeTokenId,
					Height feeDebitAppliedLastForkHeight,
					bool includeCustomNotifications)
					: m_transactionRegistry(transactionRegistry)
					, m_feeTokenId(feeTokenId)
					, m_feeDebitAppliedLastForkHeight(feeDebitAppliedLastForkHeight)
					, m_includeCustomNotifications(includeCustomNotifications)
			{}

		public:
			void publish(const WeakEntityInfoT<VerifiableEntity>& entityInfo, NotificationSubscriber& sub) const override {
				RequireKnown(entityInfo.type());

				const auto& entity = entityInfo.entity();
				auto basicEntityType = ToBasicEntityType(entityInfo.type());

				// 1. publish source change notification
				publishSourceChange(basicEntityType, sub);

				// 2. publish common notifications
				sub.notify(AccountPublicKeyNotification(entity.SignerPublicKey));

				// 3. publish entity specific notifications
				switch (basicEntityType) {
				case BasicEntityType::Block:
					return publishBlock(static_cast<const Block&>(entity), sub);

				case BasicEntityType::Transaction:
					return publishTransaction(entityInfo, sub);

				default:
					return;
				}
			}

		private:
			void publishSourceChange(BasicEntityType basicEntityType, NotificationSubscriber& sub) const {
				using Notification = SourceChangeNotification;

				switch (basicEntityType) {
				case BasicEntityType::Block:
					// set block source to zero (source ids are 1-based)
					sub.notify(Notification(Notification::SourceChangeType::Absolute, 0, Notification::SourceChangeType::Absolute, 0));
					break;

				case BasicEntityType::Transaction:
					// set transaction source (source ids are 1-based)
					sub.notify(Notification(Notification::SourceChangeType::Relative, 1, Notification::SourceChangeType::Absolute, 0));
					break;

				default:
					break;
				}
			}

			void publishBlock(const Block& block, NotificationSubscriber& sub) const {
				// raise an account public key notification
				auto blockSignerAddress = GetSignerAddress(block);
				if (blockSignerAddress != block.BeneficiaryAddress)
					sub.notify(AccountAddressNotification(block.BeneficiaryAddress));

				// raise a block type notification
				sub.notify(BlockTypeNotification(block.Type, block.Height));

				// raise an entity notification
				sub.notify(EntityNotification(block.Network, block.Version, Block::Current_Version, Block::Current_Version));

				// raise a block notification
				auto blockTransactionsInfo = CalculateBlockTransactionsInfo(block, m_transactionRegistry);
				auto blockNotification = CreateBlockNotification(block, blockSignerAddress);
				blockNotification.NumTransactions = blockTransactionsInfo.DeepCount;
				blockNotification.TotalFee = blockTransactionsInfo.TotalFee;

				sub.notify(blockNotification);

				if (IsImportanceBlock(block.Type)) {
					// raise an importance block notification only for importance blocks
					const auto& blockFooter = GetBlockFooter<ImportanceBlockFooter>(block);
					sub.notify(ImportanceBlockNotification(
							blockFooter.VotingEligibleAccountsCount,
							blockFooter.HarvestingEligibleAccountsCount,
							blockFooter.TotalVotingBalance,
							blockFooter.PreviousImportanceBlockHash));
				}

				// raise a signature notification
				sub.notify(SignatureNotification(block.SignerPublicKey, block.Signature, GetBlockHeaderDataBuffer(block)));
			}

			void publishTransaction(const WeakEntityInfoT<VerifiableEntity>& entityInfo, NotificationSubscriber& sub) const {
				const auto& transaction = static_cast<const Transaction&>(entityInfo.entity());
				const auto* pBlockHeader = entityInfo.isAssociatedBlockHeaderSet() ? &entityInfo.associatedBlockHeader() : nullptr;

				// when VerifiableEntityHeader_Reserved1 is set (currently by HarvestingUtFacade), MaxFee should be used
				auto fee = pBlockHeader && 0 == pBlockHeader->VerifiableEntityHeader_Reserved1
						? CalculateTransactionFee(pBlockHeader->FeeMultiplier, transaction)
						: transaction.MaxFee;

				BITXORCORE_LOG(trace)
						<< "[Transaction Fee Info]" << std::endl
						<< "+       pBlockHeader: " << !!pBlockHeader << std::endl
						<< "+      FeeMultiplier: " << (pBlockHeader ? pBlockHeader->FeeMultiplier : BlockFeeMultiplier()) << std::endl
						<< "+ transaction.MaxFee: " << transaction.MaxFee << std::endl
						<< "+                fee: " << fee << std::endl
						<< "+   transaction.Size: " << transaction.Size << std::endl
						<< "+   transaction.Type: " << transaction.Type;

				const auto& plugin = *m_transactionRegistry.findPlugin(transaction.Type);
				publishTransactionPreCustom(transaction, entityInfo.hash(), fee, plugin, sub);

				auto shouldApplyFeeDebitLast = !pBlockHeader || pBlockHeader->Height >= m_feeDebitAppliedLastForkHeight;
				if (m_includeCustomNotifications && shouldApplyFeeDebitLast)
					publishTransactionCustom(entityInfo, sub);

				publishTransactionPostCustom(transaction, fee, plugin, sub);

				if (m_includeCustomNotifications && !shouldApplyFeeDebitLast)
					publishTransactionCustom(entityInfo, sub);
			}

			void publishTransactionPreCustom(
					const Transaction& transaction,
					const Hash256& hash,
					Amount fee,
					const model::TransactionPlugin& plugin,
					NotificationSubscriber& sub) const {
				auto attributes = plugin.attributes();

				// raise an entity notification
				sub.notify(EntityNotification(transaction.Network, transaction.Version, attributes.MinVersion, attributes.MaxVersion));

				// raise transaction notifications
				auto signerAddress = GetSignerAddress(transaction);
				sub.notify(TransactionNotification(signerAddress, hash, transaction.Type, transaction.Deadline));
				sub.notify(TransactionDeadlineNotification(transaction.Deadline, attributes.MaxLifetime));
				sub.notify(TransactionFeeNotification(signerAddress, transaction.Size, fee, transaction.MaxFee));
			}

			void publishTransactionCustom(const WeakEntityInfoT<VerifiableEntity>& entityInfo, NotificationSubscriber& sub) const {
				CustomNotificationPublisher(m_transactionRegistry).publish(entityInfo, sub);
			}

			void publishTransactionPostCustom(
					const Transaction& transaction,
					Amount fee,
					const model::TransactionPlugin& plugin,
					NotificationSubscriber& sub) const {
				// raise a debit notification
				auto signerAddress = GetSignerAddress(transaction);
				sub.notify(BalanceDebitNotification(signerAddress, m_feeTokenId, fee));

				// raise a signature notification
				sub.notify(SignatureNotification(
						transaction.SignerPublicKey,
						transaction.Signature,
						plugin.dataBuffer(transaction),
						SignatureNotification::ReplayProtectionMode::Enabled));
			}

		private:
			const TransactionRegistry& m_transactionRegistry;
			UnresolvedTokenId m_feeTokenId;
			Height m_feeDebitAppliedLastForkHeight;
			bool m_includeCustomNotifications;
		};
	}

	std::unique_ptr<NotificationPublisher> CreateNotificationPublisher(
			const TransactionRegistry& transactionRegistry,
			UnresolvedTokenId feeTokenId,
			Height feeDebitAppliedLastForkHeight,
			PublicationMode mode) {
		switch (mode) {
		case PublicationMode::Basic:
			return std::make_unique<BasicNotificationPublisher>(transactionRegistry, feeTokenId, feeDebitAppliedLastForkHeight, false);

		case PublicationMode::Custom:
			return std::make_unique<CustomNotificationPublisher>(transactionRegistry);

		default:
			return std::make_unique<BasicNotificationPublisher>(transactionRegistry, feeTokenId, feeDebitAppliedLastForkHeight, true);
		}
	}
}}
