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
#include <typeindex>
#include <algorithm>
#include <memory>
#include <mutex>
#include <filesystem>
#include <bit>

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
		class not_found : public std::runtime_error {
		public:
			not_found(const std::string &message) : std::runtime_error(message) {}
		};

		template<class F, class R, class... argv>
		concept invocable_r = std::is_invocable_r_v<R, F, argv...>;

		template<class T>
		concept pointer = std::is_pointer_v<T>;

		template<class T>
		concept primitive = std::integral<T> || std::floating_point<T>;

		constexpr string_view throw_on_non_null_terminated(string_view str) {
			if (str[str.length() - 1] != '\0') {
				throw std::runtime_error("Detected string that is not null terminated");
			}

			return str;
		}

		constexpr char *endian_flip(char *bytes, unsigned int size) {
			if (!bytes){
				return bytes;
			}

			char *begin = bytes, *end = bytes + (size - 1);
			int offset = size % 2 == 0 ? 1 : 0;

			while (begin + offset != end) {
				char temp = *begin;
				*begin = *end;
				*end = temp;

				begin++;
				end--;
			}

			if (offset == 1) {
				char temp = *begin;
				*begin = *end;
				*end = temp;
			}

			return bytes;
		}

		constexpr std::byte endian_size_t_code(std::endian e) {
			switch (e) {
				case std::endian::little:
					return std::byte(0);
				case std::endian::big:
					return std::byte(1);
			}

			return std::byte(-1);
		}
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

		class FileMapping {

		};
	}

	namespace interoperability {
		class toolbox {
		private:
			std::variant<
				std::fstream,
				win32::Pipe,
				win32::FileMapping
			> internal_stream;

			using flag_t = std::uint32_t;
			using type_size_t = std::uint32_t;

			static constexpr flag_t std_fstream = 0;
			static constexpr flag_t win32_pipe = 1;
			static constexpr flag_t win32_file_mapping = 2;

			static constexpr flag_t type_unsigned = 	0b1000000000;

			// No unsigned bool
			static constexpr flag_t type_bool = 		0b1000000001;

			static constexpr flag_t type_char = 		0b0000000011;
			static constexpr flag_t type_short = 		0b0000000111;

			// No unsigned wchar_t
			static constexpr flag_t type_wchar_t = 		0b1000001111;

			static constexpr flag_t type_int = 			0b0000011111;
			static constexpr flag_t type_long = 		0b0000111111;
			static constexpr flag_t type_long_long = 	0b0001111111;

			// No unsigned float or unsigned double
			static constexpr flag_t type_float = 		0b101111111;
			static constexpr flag_t type_double = 		0b111111111;



			static constexpr type_size_t type_size_bool = 1;

			static constexpr type_size_t type_size_char = 1;
			static constexpr type_size_t type_size_short = 2;

			static constexpr type_size_t type_size_wchar_t = 2;

			static constexpr type_size_t type_size_int = 4;
			static constexpr type_size_t type_size_long = 8;
			static constexpr type_size_t type_size_long_long = 16;

			static constexpr type_size_t type_size_float = 4;
			static constexpr type_size_t type_size_double = 8;

			static constexpr type_size_t get_type_size(flag_t flag) {
				// Remove sign

				switch (flag) {
					case type_bool:
						return type_size_bool;
					case type_wchar_t:
						return type_size_wchar_t;
					case type_float:
						return type_size_float;
					case type_double:
						return type_size_double;
					default:
						break;
				}

				flag &= ~type_unsigned;

				switch (flag) {
					case type_char:
						return type_size_char;
					case type_short:
						return type_size_short;
					case type_int:
						return type_size_int;
					case type_long:
						return type_size_long;
					case type_long_long:
						return type_size_long_long;
					default:
						break;
				}

				return static_cast<type_size_t>(-1);
			}

			struct type_pair {
				std::type_index type;
				flag_t flag;

				type_pair() : type(typeid(void)), flag(0) {}
				type_pair(const std::type_info &ti, flag_t f) : type(ti), flag(f) {}				
			};

			class type_map_t {
			private:
				std::vector<type_pair> type_pairs;
			public:
				type_map_t(int capacity = 14) : type_pairs(capacity) {}

				std::type_index operator[](flag_t flag) {
					std::vector<type_pair>::iterator it;

					if (
						(it = std::find_if(
							this->type_pairs.begin(), this->type_pairs.end(),
							[&](const type_pair &tp) {
								return tp.flag == flag;
							}
						)
						) == this->type_pairs.end()
					) {
						throw internal::not_found("Could not find type");
					}

					return it->type;;
				}

				flag_t operator[](std::type_index type) {
					std::vector<type_pair>::iterator it;

					if (
						(it = std::find_if(
							this->type_pairs.begin(), this->type_pairs.end(),
							[&](const type_pair &tp) {
								return tp.type == type;
							}
						)
						) == this->type_pairs.end()
					) {
						throw internal::not_found("Could not find type");
					}

					return it->flag;
				}

				void push_back(const type_pair &tp) {
					this->type_pairs.push_back(tp);
				}

				void push_back(type_pair &&tp) {
					this->type_pairs.push_back(std::move(tp));
				}
			};

			static type_map_t type_map;
			static std::mutex type_map_init_mutex;
			static bool initialized;

			static void type_map_init() {
				// Multithread initialization protection in case programmer is
				// using multiple toolboxes in multiple threads

				std::unique_lock<std::mutex> type_map_init_lock(type_map_init_mutex);

				if (!initialized) {
					type_map.push_back(type_pair(typeid(bool), type_bool));
					type_map.push_back(type_pair(typeid(char), type_char));
					type_map.push_back(type_pair(typeid(unsigned char), type_unsigned | type_char));
					type_map.push_back(type_pair(typeid(short), type_short));
					type_map.push_back(type_pair(typeid(unsigned short), type_unsigned | type_short));
					type_map.push_back(type_pair(typeid(wchar_t), type_wchar_t));
					type_map.push_back(type_pair(typeid(int), type_int));
					type_map.push_back(type_pair(typeid(unsigned int), type_unsigned | type_int));
					type_map.push_back(type_pair(typeid(long), type_long));
					type_map.push_back(type_pair(typeid(unsigned long), type_unsigned | type_long));
					type_map.push_back(type_pair(typeid(long long), type_long_long));
					type_map.push_back(type_pair(typeid(unsigned long long), type_unsigned | type_long_long));
					type_map.push_back(type_pair(typeid(float), type_float));
					type_map.push_back(type_pair(typeid(double), type_double));
				}
			}

			// TODO: Clean up
			std::ios_base::openmode do_not_override_if_exists(const string &filename) {
				if (std::filesystem::exists(filename)) {
					return std::ios_base::in | std::ios_base::out | 
						   std::ios_base::binary;
				}

				return std::ios_base::in | std::ios_base::out | 
					   std::ios_base::binary | std::ios_base::trunc;
			}
		public:
			toolbox(
				const string &filename
			) : internal_stream(
				std::fstream(
					filename,
					do_not_override_if_exists(filename)
				)
			) {
				if (!std::get<std_fstream>(this->internal_stream).is_open()) {
					throw std::runtime_error(
						"Couldn't create / open internal toolbox file \"" +
						filename + "\""
					);
				}

				type_map_init();
			}

			template<internal::primitive P>
			void create(string_view name, P &&p) {
				// TODO: Do we need this?
				//internal::throw_on_non_null_terminated(name);

				switch (this->internal_stream.index()) {
					case std_fstream: {
						std::fstream &temp_ref = std::get<std::fstream>(
							this->internal_stream
						);

						// Format: type variable_name_length variable_name variable_value

						static constexpr std::byte endianness_code = 
							internal::endian_size_t_code(std::endian::native);

						temp_ref.write(
							reinterpret_cast<const char *>(&endianness_code),
							sizeof(std::byte)
						);

						flag_t flag = type_map[typeid(P)];

						temp_ref.write(
							reinterpret_cast<const char *>(&flag),
							sizeof(flag_t)
						);

						std::uint32_t variable_name_length = name.length();

						temp_ref.write(
							reinterpret_cast<const char *>(&variable_name_length),
							sizeof(unsigned int)
						);

						temp_ref.write(
							name.data(),
							variable_name_length
						);

						temp_ref.write(
							reinterpret_cast<const char *>(&p),
							sizeof(P)
						);

						break;
					}
					case win32_pipe:
						break;
					case win32_file_mapping:
						break;
				}
			}
		};

		toolbox::type_map_t toolbox::type_map;
		std::mutex toolbox::type_map_init_mutex;
		bool toolbox::initialized;
	}
}

#endif // !SE7_LINK_TOOLBOX


