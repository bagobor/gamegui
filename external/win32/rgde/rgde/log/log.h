#pragma once

#include <rgde/log/device.h>

namespace rgde
{
    namespace log
    {
        typedef boost::iostreams::stream<device::composite_dev<char> >    log_stream;
        typedef boost::iostreams::stream<device::composite_dev<wchar_t> > wlog_stream;

        //multibyte
        extern log_stream lmsg;
        extern log_stream lwrn;
        extern log_stream lerr;

        //unicode
        extern wlog_stream wlmsg;
        extern wlog_stream wlwrn;
        extern wlog_stream wlerr;

        //класс дл€ облегчени€ работы с логом:
        //  пока существует хоть один экземпл€р этого класса,
        //  выполн€етс€ перенаправление стандартных потоков (w)cout и (w)cerr в лог
        class logsystem
        {
        public:
            logsystem();
           ~logsystem();

        private:
            //гарантировано существование только одного экзмепл€ра этого класса
            class loghelper;

            static int counter;
            static loghelper *ploghelper;

        private:
            logsystem (const logsystem&);
            logsystem& operator= (const logsystem&);
        };
    }
}