#include "logger.h"


namespace logger 
{
    void InitBoostLog() {
        boost::log::add_common_attributes();
        boost::log::add_console_log(
            std::cout,
            boost::log::keywords::format = &MyFormatter,
            boost::log::keywords::auto_flush = true
        );
    }

    void MyFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
    {
        auto ts = rec[timestamp];
        strm << "{";
        strm << "\"timestamp\":" << boost::posix_time::to_iso_extended_string(*ts) << ",";
        strm << "\"data\":" << boost::json::serialize(*rec[additional_data]) << ",";
        strm << "\"message\":" << rec[boost::log::expressions::smessage];
        strm << "}";
    }

}