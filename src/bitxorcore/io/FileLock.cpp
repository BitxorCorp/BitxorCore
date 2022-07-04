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

#include "FileLock.h"
#include "bitxorcore/utils/Logging.h"
#include <thread>
#include <fcntl.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#include <sys/file.h>
#endif

namespace bitxorcore { namespace io {

	using FdType = FileLock::FdType;

	namespace {
#ifdef _MSC_VER
		const FdType Invalid_Descriptor = INVALID_HANDLE_VALUE;
		constexpr auto BxorClose = ::CloseHandle;

		auto BxorOpen(const std::string& lockFilePath) {
			return CreateFile(lockFilePath.c_str(), DELETE, FILE_SHARE_DELETE, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		}

		void BxorUnlink(const std::string& lockFilePath) {
			DeleteFile(lockFilePath.c_str());
		}
#else
		constexpr FdType Invalid_Descriptor = -1;
		constexpr auto BxorClose = ::close;

		bool TryLockFile(int fd) {
			if (0 == ::flock(fd, LOCK_NB | LOCK_EX))
				return true;

			::close(fd);
			return false;
		}

		int BxorOpen(const std::string& lockFilePath) {
			int fd = ::open(lockFilePath.c_str(), O_RDWR | O_CREAT | O_CLOEXEC | O_EXCL, 0);
			return (Invalid_Descriptor != fd) && TryLockFile(fd) ? fd : Invalid_Descriptor;
		}

		void BxorUnlink(const std::string& lockFilePath) {
			::unlink(lockFilePath.c_str());
		}
#endif

		bool TryOpenLock(const std::string& lockFilePath, FdType& fd) {
			fd = BxorOpen(lockFilePath);
			if (Invalid_Descriptor == fd) {
				BITXORCORE_LOG(warning) << "LockOpen failed: " << errno;
				return false;
			}

			return true;
		}

		void DestroyLock(const std::string& lockFilePath, FdType& fd) {
			if (Invalid_Descriptor == fd)
				return;

			BxorUnlink(lockFilePath);
			BxorClose(fd);
			fd = Invalid_Descriptor;
		}
	}

	FileLock::FileLock(const std::string& lockFilePath)
			: m_lockFilePath(lockFilePath)
			, m_fd(Invalid_Descriptor)
	{}

	FileLock::~FileLock() {
		unlock();
	}

	void FileLock::lock() {
		while (!try_lock())
			std::this_thread::yield();
	}

	bool FileLock::try_lock() {
		if (!m_spinLock.try_lock())
			return false;

		bool isLockAcquired = (Invalid_Descriptor == m_fd) && TryOpenLock(m_lockFilePath, m_fd);
		m_spinLock.unlock();
		return isLockAcquired;
	}

	void FileLock::unlock() noexcept {
		utils::SpinLockGuard guard(m_spinLock);
		DestroyLock(m_lockFilePath, m_fd);
	}
}}
