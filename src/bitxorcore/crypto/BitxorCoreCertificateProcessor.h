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
#include "CertificateUtils.h"

struct x509_store_ctx_st;

namespace bitxorcore { namespace crypto {

	/// BitxorCore-specific certificate processor.
	/// \note This is specific to processing bitxorcore certificates and is not general purpose.
	class BitxorCoreCertificateProcessor {
	public:
		/// Gets the number of certificates in the chain.
		size_t size() const;

		/// Gets the parsed certificate information at \a depth.
		/// \note Depth is 0-based starting with the root certificate.
		const CertificateInfo& certificate(size_t depth) const;

	public:
		/// Verifies the current certificate in \a certificateStoreContext given \a preverified result.
		bool verify(bool preverified, x509_store_ctx_st& certificateStoreContext);

	private:
		bool verifyUnverifiedRoot(x509_st& certificate, int errorCode);
		bool push(x509_st& certificate);

	public:
		std::vector<CertificateInfo> m_certificateInfos;
	};
}}
