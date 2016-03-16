#pragma once

namespace gui
{
	namespace platform_win32
	{
		class log : public ::gui::log
		{
		public:
			log() : m_hFile(INVALID_HANDLE_VALUE)
			{
				wchar_t	wpath[MAX_PATH];
				// init app data path
				SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wpath);

				char buff[256];
				int size = GetModuleFileNameA(NULL, buff, 256);

				std::string path(buff, size);

				int pos = path.find_last_of('\\');

				path = path.substr(0, pos);

				std::string log = path + "/guitest.log";
				m_hFile = CreateFileA(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
			}

			~log() {
				if (m_hFile != INVALID_HANDLE_VALUE)
					CloseHandle(m_hFile);
			}

			void write(log::level l, const std::string& msg)
			{
				static const char* type2str[log::log_levels_num] = { "sys", "msg", "wrn", "ERR", "CRITICAL" };
				const char* type = type2str[l];

				SYSTEMTIME st;
				GetLocalTime(&st);

				static char con_timestamp[32] = { 0 };
				_snprintf(con_timestamp, 32, "[%02d:%02d:%02d][%s] ", st.wHour, st.wMinute, st.wSecond, type);
				std::cout << con_timestamp << msg.c_str() << std::endl;

				if (m_hFile == INVALID_HANDLE_VALUE)
					return;

				static char timestamp[32] = { 0 };
				_snprintf(timestamp, 32, "[%04d.%02d.%02d %02d:%02d:%02d][%s] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, type);

				std::string m = timestamp + msg + "\n";
				DWORD len = (DWORD)m.length();
				WriteFile(m_hFile, m.c_str(), len, &len, 0);
			}

			HANDLE m_hFile;
		};
	}
}

