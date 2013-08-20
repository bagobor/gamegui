#pragma once

namespace rgde
{
    namespace log
    {
        namespace device
        {

            //вспомогательный класс дял работы с файлом лога
            class file_wrapper
            {
            public:
                file_wrapper ();
                file_wrapper (const std::string &filename);
               ~file_wrapper ();

                void open (const std::string &filename);
                void close ();

	            std::streamsize write(const char *s, std::streamsize n);
	            std::streamsize write(const wchar_t *s, std::streamsize n);

            private:
                file_wrapper (const file_wrapper&);
                file_wrapper& operator= (const file_wrapper&);

                std::ofstream m_file;
            };
            typedef boost::shared_ptr<file_wrapper> PFileWrapper;

            //вспомогательный класс для работы с потоками
            template <typename CHAR>
            class ostream_wrapper
            {
            public:
				//int text_color;
                typedef CHAR char_type;

                ostream_wrapper ():
                    m_out(0)//, text_color(0xFFFFFFFF)
                {
                }

                ostream_wrapper (std::basic_streambuf<char_type> *sbuf):
                    m_out(0)
                {
                    open(sbuf);
                }

               ~ostream_wrapper ()
                {
                    close();
                }

               void open (std::basic_streambuf<char_type> *sbuf)
                {
                    if (m_out.rdbuf())
                        close();

                    m_out.rdbuf(sbuf);
                }

                void close ()
                {
                    if (m_out.rdbuf())
                    {
                        m_out.rdbuf(0);
                    }
                }

	            std::streamsize write(const char_type *s, std::streamsize n)
	            {
                    if (!m_out.rdbuf())
                        return n; //или следует вернуть 0 (хз как буст отреагирует)?

					// Add color output!
					//HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE ); // Make a handle
					//text_color 
					//SetConsoleTextAttribute( handle, text_color);
					//SetConsoleTextAttribute( handle, FOREGROUND_RED ); // Set the color of the text

                    m_out << std::basic_string<char_type>(s, n);
		            return n;
	            }

            private:
                ostream_wrapper (const ostream_wrapper<char_type>&);
                ostream_wrapper& operator= (const ostream_wrapper<char_type>&);

                std::basic_ostream<char_type> m_out;
            };
            typedef boost::shared_ptr<ostream_wrapper<char> > POStreamWrapperA;
            typedef boost::shared_ptr<ostream_wrapper<wchar_t> > POStreamWrapperW;

            //"устройство" для записи в файл
            template <typename CHAR>
            class file_dev
            {
            public:
	            typedef CHAR char_type;
	            typedef boost::iostreams::sink_tag category;

                file_dev (const file_dev &inst)
                {
                    m_pfile = inst.m_pfile;
                }

                file_dev(PFileWrapper pfile)
                {
                    m_pfile = pfile;
                }

	            std::streamsize write(const char_type *s, std::streamsize n)
	            {
		            return m_pfile->write(s,n);
	            }

                void open (const std::string &filename)
                {
                    m_pfile->open(filename);
                }

                void close ()
                {
                    m_pfile->close();
                }

            private:
                PFileWrapper m_pfile;
            };

            //"устройство" для вывода в консоль
            template <typename CHAR>
            class screen_dev
            {
            public:
	            typedef CHAR char_type;
	            typedef boost::iostreams::sink_tag category;

                screen_dev(const screen_dev &inst)
                {
                    m_postream = inst.m_postream;
                }

                screen_dev(boost::shared_ptr<ostream_wrapper<char_type> > postream)
                {
                    m_postream = postream;
                }

	            std::streamsize write(const char_type *s, std::streamsize n)
	            {
                    m_postream->write(s, n);
		            return n;
	            }

                void open (std::streambuf *sbuf)
                {
                    m_postream->open(sbuf);
                }

                void close ()
                {
                    m_postream->close();
                }

            private:
                boost::shared_ptr<ostream_wrapper<char_type> > m_postream;
            };

			void text_write(const char* str, std::streamsize n);
			void text_write(const wchar_t* str, std::streamsize n);

            //"устройство" для вывода в output VisualStudio
            template <typename CHAR>
            class output_dev
            {
            public:
	            typedef CHAR char_type;
	            typedef boost::iostreams::sink_tag category;

                //реализация метода write по умолчанию использует OutputDebugStringA
	            std::streamsize write(const char_type *s, std::streamsize n)
	            {
					text_write(s, n);
                    return n;
	            }
            };

            //"устройство" - контейнер. позволяет вести вывод сразу в несколько (сейчас в три) устройств
            template <typename CHAR>
            class composite_dev
            {
            public:
	            typedef CHAR char_type;
	            typedef boost::iostreams::sink_tag category;

                composite_dev (const composite_dev<char_type> &inst):
                    m_stream1(inst.m_stream1.rdbuf()),
                    m_stream2(inst.m_stream2.rdbuf()),
                    m_stream3(inst.m_stream3.rdbuf())
                {
                }

                composite_dev
                (
                    std::basic_ostream<char_type> &stream1,
                    std::basic_ostream<char_type> &stream2,
                    std::basic_ostream<char_type> &stream3
                ):
                    m_stream1(stream1.rdbuf()),
                    m_stream2(stream2.rdbuf()),
                    m_stream3(stream3.rdbuf())
                {
                }

	            std::streamsize write(const char_type *s, std::streamsize n)
	            {
                    std::basic_string<char_type> str(s,n);
                    m_stream1 << str;
                    m_stream2 << str;
                    m_stream3 << str;

                    m_stream1.flush();
                    m_stream2.flush();
                    m_stream3.flush();
		            return n;
	            }

            private:
                std::basic_ostream<char_type> m_stream1;
                std::basic_ostream<char_type> m_stream2;
                std::basic_ostream<char_type> m_stream3;
            };
        }
    }
}