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

#include "BitxorCoreCertificateProcessor.h"
#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/exceptions.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <openssl/x509.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace bitxorcore { namespace crypto {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#endif

	namespace {
		size_t GetChainSize(X509_STORE_CTX& certificateStoreContext) {
			return static_cast<size_t>(sk_X509_num(X509_STORE_CTX_get0_chain(&certificateStoreContext)));
		}
	}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

	size_t BitxorCoreCertificateProcessor::size() const {
		return m_certificateInfos.size();
	}

	const CertificateInfo& BitxorCoreCertificateProcessor::certificate(size_t depth) const {
		return m_certificateInfos[depth];
	}

	bool BitxorCoreCertificateProcessor::verify(bool preverified, X509_STORE_CTX& certificateStoreContext) {
		// reject all certificate chains that are not composed of two certificates
		auto chainSize = GetChainSize(certificateStoreContext);
		if (2 != chainSize) {
			BITXORCORE_LOG(warning) << "rejecting certificate chain with size " << chainSize;
			return false;
		}

		auto* pCertificate = X509_STORE_CTX_get_current_cert(&certificateStoreContext);
		if (!pCertificate)
			BITXORCORE_THROW_INVALID_ARGUMENT("rejecting certificate chain with no active certificate");

		if (preverified) {
			if (push(*pCertificate))
				return true;

			X509_STORE_CTX_set_error(&certificateStoreContext, X509_V_ERR_APPLICATION_VERIFICATION);
			return false;
		}

		auto errorCode = X509_STORE_CTX_get_error(&certificateStoreContext);
		return verifyUnverifiedRoot(*pCertificate, errorCode);
	}

	bool BitxorCoreCertificateProcessor::verifyUnverifiedRoot(X509& certificate, int errorCode) {
		// only perform custom verification for root (CA) certificate
		if (!m_certificateInfos.empty()) {
			BITXORCORE_LOG(warning) << "rejecting certificate chain with unverified non-root certificate";
			return false;
		}

		// only verify self signed certificates
		if (X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN != errorCode) {
			BITXORCORE_LOG(warning) << "rejecting certificate chain with unverified unexpected error " << errorCode;
			return false;
		}

		if (!VerifySelfSigned(certificate)) {
			BITXORCORE_LOG(warning) << "rejecting certificate chain with improperly self-signed root certificate";
			return false;
		}

		return true;
	}

	bool BitxorCoreCertificateProcessor::push(X509& certificate) {
		CertificateInfo certificateInfo;
		if (!TryParseCertificate(certificate, certificateInfo)) {
			BITXORCORE_LOG(warning) << "rejecting certificate chain due to certificate parse failure";
			return false;
		}

		m_certificateInfos.push_back(certificateInfo);
		return true;
	}
}}
