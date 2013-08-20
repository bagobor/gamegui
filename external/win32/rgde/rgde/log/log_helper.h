// !!! Attension! Not "#pragma once" is used here !!!
// -= this file is intened only to be included from log.cpp =-

namespace rgde
{
	namespace log
	{
		class logsystem::loghelper
		{
		public:
			loghelper();
			~loghelper();

		private:
			//указатели на стандартные потоки
			std::streambuf  *coutsbuf;
			std::wstreambuf *wcoutsbuf;

			std::streambuf  *cerrsbuf;
			std::wstreambuf *wcerrsbuf;

			static std::string generateLogName();

		private:
			loghelper (const loghelper&);
			loghelper& operator= (const loghelper&);
		};
	}
}