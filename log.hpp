#pragma once
#include <fstream>
#include <mutex>

namespace glog
{
    int log(std::string level, std::string data, std::string module);

    int __stdcall log_str(char *level, char *data, char *module);

} // namespace log
