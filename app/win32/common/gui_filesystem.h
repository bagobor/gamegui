#pragma once

using namespace gui;

class gui_filesystem : public gui::filesystem
{
public:
	gui_filesystem(const std::string& basedir) 
	{
		char buff[256];
		int size = GetModuleFileNameA(NULL, buff, 256);

		std::string path(buff, size);

		int pos = path.find_last_of('\\');

		m_basedir = path.substr(0, pos) + "/.." + basedir;

		//m_basedir = m_basedir + "data/";
	}

	~gui_filesystem() {
	}

	virtual std::string load_text(const std::string& filename) 	{
		std::string out;

		std::string full_filename = m_basedir + filename;

		if (FILE *fp = fopen(full_filename.c_str(), "rb"))
		{
			fseek(fp, 0, SEEK_END);
			out.resize(ftell(fp));
			rewind(fp);
			fread(&out[0], 1, out.size(), fp);
			fclose(fp);			
		}
		return out;
	}

	virtual data_ptr load_binary(const std::string& filename) {
		data_ptr out;

		std::string full_filename = m_basedir + filename;
		if (FILE *fp = fopen(full_filename.c_str(), "rb"))
		{
			fseek(fp, 0, SEEK_END);
			size_t size = ftell(fp);
			rewind(fp);

			if (size == 0)
				return out;

			out = data_ptr(new data);
			out->ptr = new char[size];
			out->size = size;

			fread(out->ptr, 1, size, fp);
			fclose(fp);
		}

		return out;
	}

	const std::string& root() { return m_basedir; }

protected:
	std::string m_basedir;
};