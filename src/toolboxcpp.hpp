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

namespace SE7 {
	namespace internal {
		template<class F, class R, class... argv>
		concept invocable_r = std::is_invocable_r_v<R, F, argv...>;
	}

	namespace win32 {
#ifdef UNICODE
		using string = std::wstring;
#else
		using string = std::string;
#endif

		class HandleCreationFailure : public std::runtime_error {
		public:
			const DWORD getLastErrorCode;

			HandleCreationFailure() : 
				std::runtime_error("Handle could not be created"),
				getLastErrorCode(GetLastError())
				{}
		};

		class Handle {
		private:
			HANDLE internalHandle;
		public:
			template<
				class... argv, 
				internal::invocable_r<HANDLE, argv...> WIN32_FUNC
			>
			Handle(WIN32_FUNC &&func, argv&&... args) : 
				internalHandle(func(std::forward(args)...)){
				if (this->internalHandle == INVALID_HANDLE_VALUE) {
					throw HandleCreationFailure();
				}
				
			}

			HANDLE GetInternalHandle() const {
				return this->internalHandle;
			}

			operator HANDLE() const {
				return this->internalHandle;
			}

			~Handle() {
				CloseHandle(this->internalHandle);
			}
		};

		enum class FileDesiredAccess {
			GenericRead = GENERIC_READ,
			GenericWrite = GENERIC_WRITE
		};

		enum class FileShareMode {
			NoShare = 0x00000000,
			Read = 0x00000001,
			Write = 0x00000002,
			Delete = 0x00000004
		};

		using SecurityAttributes = SECURITY_ATTRIBUTES;
		using SecurityAttributesPointer = PSECURITY_ATTRIBUTES;

		namespace FileCreationTags {
			struct CreateAlways {}; 	// 2
			struct CreateNew {};    	// 1
			struct OpenAlways {};   	// 4
			struct OpenExisting {}; 	// 3
			struct TruncateExisting {}; // 5
		}

		using FileCreationDisposition = std::variant<
			FileCreationTags::CreateNew,
			FileCreationTags::CreateAlways,
			FileCreationTags::OpenExisting,
			FileCreationTags::OpenAlways,
			FileCreationTags::TruncateExisting
		>;

		enum class FileAttributes {
			// TODO: There are more than just these, add them.

			Archive = FILE_ATTRIBUTE_ARCHIVE,
			Encrypted = FILE_ATTRIBUTE_ENCRYPTED,
			Hidden = FILE_ATTRIBUTE_HIDDEN,
			Normal = FILE_ATTRIBUTE_NORMAL,
			Offline = FILE_ATTRIBUTE_OFFLINE,
			ReadOnly = FILE_ATTRIBUTE_READONLY,
			System = FILE_ATTRIBUTE_SYSTEM,
			Temporary = FILE_ATTRIBUTE_TEMPORARY
		};

		enum class FileFlags {
			BackupSemantics = FILE_FLAG_BACKUP_SEMANTICS,
			DeleteOnClose = FILE_FLAG_DELETE_ON_CLOSE,
			NoBuffering = FILE_FLAG_NO_BUFFERING,
			OpenNoRecall = FILE_FLAG_OPEN_NO_RECALL,
			OpenReparsePoint = FILE_FLAG_OPEN_REPARSE_POINT,
			Overlapped = FILE_FLAG_OVERLAPPED,
			POSIXSemantics = FILE_FLAG_POSIX_SEMANTICS,
			RandomAccess = FILE_FLAG_RANDOM_ACCESS,
			SessionAware = FILE_FLAG_SESSION_AWARE,
			SequentialScan = FILE_FLAG_SEQUENTIAL_SCAN,
			WriteThrough = FILE_FLAG_WRITE_THROUGH
		};
		
		// TODO: SQOS



		class File : public Handle {
		private:
			static DWORD GetFileDisposition(
				const FileCreationDisposition fileCreationDisposition
			) {
				return fileCreationDisposition.index() + 1;
			}
		public:
			File(
				const string &fileName,
				FileDesiredAccess fileReadAccess,
				FileShareMode fileShareMode,
				SecurityAttributesPointer securityAttributesPointer,
				const FileCreationDisposition fileCreationDisposition,
				DWORD flagsAndAttributes,
				std::optional<File> templateFile
			) : Handle(
				CreateFile,
				fileName.c_str(),
				static_cast<DWORD>(fileReadAccess),
				static_cast<DWORD>(fileShareMode),
				securityAttributesPointer,
				GetFileDisposition(fileCreationDisposition),
				flagsAndAttributes,
				templateFile == std::nullopt ? NULL : templateFile.value()
			) {

			}
		};
	}
}

#endif // !SE7_LINK_TOOLBOX


