#include "logger.h"

void MyFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    // ����� ��������� ������ � ������ �������, ����� �� ������
    auto ts = rec[timestamp];
    strm << "{";
    strm << "\"timestamp\":" << boost::posix_time::to_iso_extended_string(*ts) << ",";
    strm << "\"data\":" << boost::json::serialize(*rec[additional_data]) << ",";
    strm << "\"message\":" << rec[boost::log::expressions::smessage];
    strm << "}";
}