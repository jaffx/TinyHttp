#ifndef XYQ_H
#define XYQ_H
#include <sys/stat.h>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ctime>
namespace xyq
{
    int divide_str_by_separator(const char *inbuf, char *prefix, char *rest, const char *sep, int max_prefix_len, int max_rest_len);
    int get_prefix_by_separator(const char *inbuf, char *prefix, const char *sep, int max_prefix_len);
    int get_prefix_by_separator(const char *inbuf, char *prefix, char sep, int max_prefix_len);
    size_t get_file_size(const char *file_path);
    std::string get_time_now();
};
#endif