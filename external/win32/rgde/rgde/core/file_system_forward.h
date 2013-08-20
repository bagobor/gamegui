#pragma once

namespace rgde
{
	namespace core
	{
		namespace vfs
		{
			class system;
			class path;

			struct stream;
			typedef boost::shared_ptr<stream> stream_ptr;
			
			struct istream;
			typedef boost::shared_ptr<istream> istream_ptr;

			struct ostream;
			typedef boost::shared_ptr<ostream> ostream_ptr;

			std::wstring get_extension(const std::wstring& filename);
			std::wstring strip_extension(const std::wstring& filename);

			std::string get_extension(const std::string& filename);
			std::string strip_extension(const std::string& filename);
		}
	}
}