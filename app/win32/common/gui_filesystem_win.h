#pragma once

namespace gui
{
	namespace platform_win32
	{
		class filesystem : public gui::filesystem
		{
		public:
			explicit filesystem(const std::string& basedir)
			{
				char buff[256];
				int size = GetModuleFileNameA(NULL, buff, 256);

				std::string path(buff, size);
				int pos = path.find_last_of('\\');
				std::string ui_root_dir = path.substr(0, pos) + "/.." + basedir;
				m_roots.push_back(ui_root_dir);
			}

			virtual std::string load_text(const std::string& filename) {
				std::string out;

				std::string full_filename = get_root_dir(0) + filename;

				FILE *fp = fopen(full_filename.c_str(), "rb");
				if (!fp) return out;

				fseek(fp, 0, SEEK_END);
				out.resize(ftell(fp));
				rewind(fp);
				fread(&out[0], 1, out.size(), fp);
				fclose(fp);

				return out;
			}

			virtual gui::data_ptr load_binary(const std::string& filename) {
				gui::data_ptr out;

				std::string full_filename = get_root_dir(0) + filename;
				FILE *fp = fopen(full_filename.c_str(), "rb");
				if (!fp) return out;

				fseek(fp, 0, SEEK_END);
				size_t size = ftell(fp);
				rewind(fp);

				if (size == 0)
					return out;

				out = gui::data_ptr(new gui::data);
				out->ptr = new char[size];
				out->size = size;

				fread(out->ptr, 1, size, fp);
				fclose(fp);

				return out;
			}
		};
	}
}