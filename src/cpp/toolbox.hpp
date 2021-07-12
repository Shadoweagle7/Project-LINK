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

namespace SE7 {
	namespace internal {
		template<class F, class R, class... argv>
		concept invocable_r = std::is_invocable_r_v<R, F, argv...>;
	}

	namespace win32 {
#ifdef UNICODE
		using string = std::wstring;
		using string_view = std::wstring_view;
#else
		using string = std::string;
		using string_view = std::string_view;
#endif

		class Pipe {
		private:
			HANDLE internalHandle;
			const string &internalName;
			const DWORD internalOpenMode;
			const DWORD internalPipeMode;
			const DWORD internalMaxInstances;
			const DWORD internalOutBufferSize;
			const DWORD internalInBufferSize;
			const DWORD internalDefaultTimeOut;
			const LPSECURITY_ATTRIBUTES internalPSecurityAttributes;
		public:
			struct PipeAccess {
				static constexpr DWORD InBound = 0x00000001;
				static constexpr DWORD OutBound = 0x00000002;
				static constexpr DWORD Duplex = 0x00000003;
			};

			struct FileFlag {
				static constexpr DWORD FirstPipeInstance = 0x00080000;
				static constexpr DWORD Overlapped = 0x40000000;
				static constexpr DWORD WriteThrough = 0x80000000;
			};

			struct SecurityAccess {
				static constexpr DWORD WriteDAC = 0x00040000L;
				static constexpr DWORD WriteOwner = 0x00080000L;
				static constexpr DWORD AccessSystemSecurity = 0x01000000L;
			};

			struct StreamType {
				static constexpr DWORD Byte = 0x00000000;
				static constexpr DWORD Message = 0x00000004;
			};

			struct ReadMode {
				static constexpr DWORD Byte = 0x00000000;
				static constexpr DWORD Message = 0x00000002;
			};

			Pipe(
				// Using const std::string & instead of std::string_view because std::string_view is not
				// designed to be automatically null terminated like std::string is,
				// and we need to guarantee that when passing into CreateNamedPipe()
				const string &name,
				DWORD                 openMode,
				DWORD                 pipeMode,
				DWORD                 maxInstances,
				DWORD                 outBufferSize,
				DWORD                 inBufferSize,
				DWORD                 defaultTimeOut,
				LPSECURITY_ATTRIBUTES pSecurityAttributes
			) : internalHandle(
				CreateNamedPipe(
					name.c_str(),
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

			const string &GetName() const noexcept { return this->internalName; }
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


