#ifndef XYQ_H
#define XYQ_H
namespace xyq{
    int divide_str_by_separator(const char *inbuf, char *prefix, char *rest, char *sep, int max_prefix_len, int max_rest_len);
    int get_prefix_by_separator(const char *inbuf, char *prefix, char *sep, int max_prefix_len);
    int get_prefix_by_separator(const char *inbuf, char *prefix, char sep, int max_prefix_len);
};
#endif