#pragma once

#include <rgde/log/util.h>


namespace rgde
{
    namespace log
    {
        namespace filter
        {
            template <typename CHAR>
            struct html_time
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str = "<b>[" + util::get_current_time() + "]</b>&nbsp;";

                    std::basic_string<char_type> output(str.begin(), str.end());
                    output.append(s, s+n);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(html_time<char>,0)
            BOOST_IOSTREAMS_PIPABLE(html_time<wchar_t>,0)

            template <typename CHAR>
            struct text_time
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str("[" + util::get_current_time() + "] ");

                    std::basic_string<char_type> output(str.begin(), str.end());
                    output.append(s, s+n);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(text_time<char>,0)
            BOOST_IOSTREAMS_PIPABLE(text_time<wchar_t>,0)

            template <typename CHAR>
            struct html_msg
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str1("<b class=id1>message:</b>&nbsp;<i>");
                    std::string str2("</i>");

                    std::basic_string<char_type> output(str1.begin(), str1.end());
                    std::basic_string<char_type> end(str2.begin(), str2.end());
                    output.append(s, s+n);
                    output.append(end);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(html_msg<char>,0)
            BOOST_IOSTREAMS_PIPABLE(html_msg<wchar_t>,0)

            template <typename CHAR>
            struct text_msg
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str("message: ");

                    std::basic_string<char_type> output(str.begin(), str.end());
                    output.append(s, s+n);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(text_msg<char>,0)
            BOOST_IOSTREAMS_PIPABLE(text_msg<wchar_t>,0)

            template <typename CHAR>
            struct html_wrn
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str1("<b class=id1>warning:</b>&nbsp;<i>");
                    std::string str2("</i>");

                    std::basic_string<char_type> output(str1.begin(), str1.end());
                    std::basic_string<char_type> end(str2.begin(), str2.end());
                    output.append(s, s+n);
                    output.append(end);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(html_wrn<char>,0)
            BOOST_IOSTREAMS_PIPABLE(html_wrn<wchar_t>,0)

            template <typename CHAR>
            struct text_wrn
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str("warning: ");

                    std::basic_string<char_type> output(str.begin(), str.end());
                    output.append(s, s+n);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(text_wrn<char>,0)
            BOOST_IOSTREAMS_PIPABLE(text_wrn<wchar_t>,0)

            template <typename CHAR>
            struct html_err
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str1("<b class=id1>error:</b>&nbsp;<i>");
                    std::string str2("</i>");

                    std::basic_string<char_type> output(str1.begin(), str1.end());
                    std::basic_string<char_type> end(str2.begin(), str2.end());
                    output.append(s, s+n);
                    output.append(end);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(html_err<char>,0)
            BOOST_IOSTREAMS_PIPABLE(html_err<wchar_t>,0)

            template <typename CHAR>
            struct text_err
            {
                typedef CHAR char_type;
                typedef boost::iostreams::multichar_output_filter_tag category;

                template<typename Sink>
                std::streamsize write(Sink &snk, const char_type *s, std::streamsize n)
                {
                    std::string str("error: ");

                    std::basic_string<char_type> output(str.begin(), str.end());
                    output.append(s, s+n);

                    boost::iostreams::write(snk, output.c_str(), (std::streamsize)output.size());
                    return n;
                }
            };
            BOOST_IOSTREAMS_PIPABLE(text_err<char>,0)
            BOOST_IOSTREAMS_PIPABLE(text_err<wchar_t>,0)
        }
    }
}