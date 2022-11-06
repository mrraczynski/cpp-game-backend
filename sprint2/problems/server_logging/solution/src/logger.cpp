#include "logger.h"

#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time.hpp>

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value)

void MyFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    // чтобы поставить логеры в равные условия, уберём всё лишнее
    auto ts = rec[timestamp];
    strm << "{";
    strm << "\"timestamp\":" << boost::posix_time::to_iso_extended_string(*ts) << ",";
    strm << "\"data\":" << boost::json::serialize(*rec[additional_data]) << ",";
    strm << "\"message\":" << rec[boost::log::expressions::smessage];
    strm << "}";
}