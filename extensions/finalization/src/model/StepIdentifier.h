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

#pragma once
#include "bitxorcore/crypto_voting/BmKeyIdentifier.h"
#include "bitxorcore/model/FinalizationRound.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	// region step identifier

	/// Finalization stages.
	enum class FinalizationStage : uint32_t {
		/// Prevote stage.
		Prevote,

		/// Precommit stage.
		Precommit,

		/// Number of stages.
		Count
	};

	/// Finalization step identifier.
	struct StepIdentifier {
	public:
		struct FinalizationPointStage_tag {};
		using FinalizationPointStage = utils::BaseValue<uint32_t, FinalizationPointStage_tag>;

	public:
		/// Creates a default step identifier.
		StepIdentifier();

		/// Creates a step identifier from \a epoch, \a point and \a stage.
		StepIdentifier(FinalizationEpoch epoch, FinalizationPoint point, FinalizationStage stage);

	public:
		/// Finalization epoch.
		FinalizationEpoch Epoch;

		/// Finalization point and stage.
		FinalizationPointStage PointStage;

	public:
		/// Gets the finalization round.
		model::FinalizationRound Round() const;

		/// Gets the finalization stage.
		model::FinalizationStage Stage() const;

	public:
		/// Returns \c true if this step identifier is equal to \a rhs.
		bool operator==(const StepIdentifier& rhs) const;

		/// Returns \c true if this step identifier is not equal to \a rhs.
		bool operator!=(const StepIdentifier& rhs) const;

		/// Returns \c true if this step identifier is less than \a rhs.
		bool operator<(const StepIdentifier& rhs) const;

		/// Returns \c true if this step identifier is less than or equal to \a rhs.
		bool operator<=(const StepIdentifier& rhs) const;

		/// Returns \c true if this step identifier is greater than \a rhs.
		bool operator>(const StepIdentifier& rhs) const;

		/// Returns \c true if this step identifier is greater than or equal to \a rhs.
		bool operator>=(const StepIdentifier& rhs) const;
	};

	/// Insertion operator for outputting \a stepIdentifier to \a out.
	std::ostream& operator<<(std::ostream& out, const StepIdentifier& stepIdentifier);

	// endregion

#pragma pack(pop)

	/// Converts \a stepIdentifier to bm key identifier.
	crypto::BmKeyIdentifier StepIdentifierToBmKeyIdentifier(const StepIdentifier& stepIdentifier);
}}
