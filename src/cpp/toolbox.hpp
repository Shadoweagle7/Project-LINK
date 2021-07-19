#ifndef SE7_LINK_TOOLBOX
#define SE7_LINK_TOOLBOX

#include <Windows.h>
#include <concepts>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <optional>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <any>
#include <stdexcept>
#include <vector>
#include <initializer_list>
#include <functional>
#include <map>
#include <typeindex>
#include <algorithm>
#include <memory>

#ifdef UNICODE
using string = std::wstring;
using string_view = std::wstring_view;
#else
using string = std::string;
using string_view = std::string_view;
#endif

#if !defined(x64) && !defined(x86)
#define x64
#endif

namespace SE7 {
	namespace internal {
		template<class F, class R, class... argv>
		concept invocable_r = std::is_invocable_r_v<R, F, argv...>;

		template<class T>
		concept pointer = std::is_pointer_v<T>;

		constexpr string_view throw_on_non_null_terminated(string_view str) {
			if (str[str.length() - 1] != '\0') {
				throw std::runtime_error("Detected string that is not null terminated");
			}

			return str;
		}
	}

	namespace interoperability {
		

		class datafile {
		private:
			std::fstream internal_stream;
		public:
			datafile(
				const string &filename, 
				std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out
			) : internal_stream(filename, mode | std::ios_base::binary) {}


		};
	}

	// Naming convention for Win32 API is UpperCase, in contrast to everything else,
	// which is in snake_case. Template types are in UpperCase (template<class ClassType>),
	// but concepts are still in snake_case.
	namespace win32 {
		class Pipe {
		private:
			HANDLE internalHandle;
			const string internalName;
			const DWORD internalOpenMode;
			const DWORD internalPipeMode;
			const DWORD internalMaxInstances;
			const DWORD internalOutBufferSize;
			const DWORD internalInBufferSize;
			const DWORD internalDefaultTimeOut;
			const LPSECURITY_ATTRIBUTES internalPSecurityAttributes;
		public:
			struct PipeAccess {
				static constexpr DWORD InBound = PIPE_ACCESS_INBOUND;
				static constexpr DWORD OutBound = PIPE_ACCESS_OUTBOUND;
				static constexpr DWORD Duplex = PIPE_ACCESS_DUPLEX;
			};

			struct FileFlag {
				static constexpr DWORD FirstPipeInstance = FILE_FLAG_FIRST_PIPE_INSTANCE;
				static constexpr DWORD Overlapped = FILE_FLAG_OVERLAPPED;
				static constexpr DWORD WriteThrough = FILE_FLAG_WRITE_THROUGH;
			};

			struct SecurityAccess {
				static constexpr DWORD WriteDAC = WRITE_DAC;
				static constexpr DWORD WriteOwner = WRITE_OWNER;
				static constexpr DWORD AccessSystemSecurity = ACCESS_SYSTEM_SECURITY;
			};

			struct StreamType {
				static constexpr DWORD Byte = PIPE_TYPE_BYTE;
				static constexpr DWORD Message = PIPE_TYPE_MESSAGE;
			};

			struct ReadMode {
				static constexpr DWORD Byte = PIPE_READMODE_BYTE;
				static constexpr DWORD Message = PIPE_READMODE_MESSAGE;
			};

			struct WaitMode {
				static constexpr DWORD Wait = PIPE_WAIT;
				static constexpr DWORD NoWait = PIPE_NOWAIT;
			};

			struct RemoteClientMode {
				static constexpr DWORD Accept = PIPE_ACCEPT_REMOTE_CLIENTS;
				static constexpr DWORD Reject = PIPE_REJECT_REMOTE_CLIENTS;
			};

			Pipe(
				string_view name,
				DWORD                 openMode,
				DWORD                 pipeMode,
				DWORD                 maxInstances,
				DWORD                 outBufferSize,
				DWORD                 inBufferSize,
				DWORD                 defaultTimeOut,
				LPSECURITY_ATTRIBUTES pSecurityAttributes
			) : internalHandle(
				CreateNamedPipe(
					internal::throw_on_non_null_terminated(name).data(),
					openMode,
					pipeMode,
					maxInstances,
					outBufferSize,
					inBufferSize,
					defaultTimeOut,
					pSecurityAttributes
				)
			),
			internalName(name),
			internalOpenMode(openMode),
			internalPipeMode(pipeMode),
			internalMaxInstances(internalMaxInstances),
			internalOutBufferSize(outBufferSize),
			internalInBufferSize(inBufferSize),
			internalDefaultTimeOut(defaultTimeOut),
			internalPSecurityAttributes(pSecurityAttributes)
			{}

			string_view GetName() const noexcept { return this->internalName; }
			const DWORD GetOpenMode() const noexcept { return this->internalOpenMode; }
			const DWORD GetPipeMode() const noexcept { return this->internalPipeMode; }
			const DWORD GetMaxInstances() const noexcept { return this->internalMaxInstances; }
			const DWORD GetOutBufferSize() const noexcept { return this->internalOutBufferSize; }
			const DWORD GetInBufferSize() const noexcept { return this->internalInBufferSize; }
			const DWORD GetDefaultTimeout() const noexcept { return this->internalDefaultTimeOut; }
			const LPSECURITY_ATTRIBUTES GetPointerToSecurityAttributes() const noexcept {
				return this->internalPSecurityAttributes;
			};

			template<size_t N, size_t M>
			BOOL Call(
				std::array<std::byte, N> inBuffer,
				std::array<std::byte, M> &outBuffer,
				DWORD &refBytesRead,
				DWORD timeout
			) {
				return CallNamedPipe(
					this->internalName.c_str(),
					static_cast<void *>(inBuffer.data()),
					inBuffer.size() * sizeof(std::byte),
					outBuffer.size() * sizeof(std::byte),
					&refBytesRead,
					timeout
				);
			}

			~Pipe() {
				CloseHandle(this->internalHandle);
			}
		};
	}
}

#endif // !SE7_LINK_TOOLBOX


