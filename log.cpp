#pragma once
#include "log.hpp"
std::mutex log_mutex;
#define LOG_FILE ".\\log.log"
int __stdcall glog::log(std::string level, std::string data, std::string module)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL)
    {
        file = fopen(LOG_FILE, "w");
        return -1;
    }
    time_t now = time(0);
    tm *ltm = localtime(&now);
    fprintf(file, "[%d-%02d-%02d %02d:%02d:%02d] [%s] [%s] %s\n", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec, level.c_str(), module.c_str(), data.c_str());
    fclose(file);
    return 0;
}
int __stdcall glog::log_str(char *level, char *data, char *module)
{
    return glog::log(std::string(level), std::string(data), std::string(module));
}
