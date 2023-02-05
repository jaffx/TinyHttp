#include "xyq.h"

int xyq::divide_str_by_separator(const char *inbuf, char *prefix, char *rest, const char *sep, int max_prefix_len, int max_rest_len)
{
    /*
    @function
        将一个字符串分成两部分呢，前一部分放在prefix中，后一部分放在rest中
    @return
        成功返回0，失败返回-1
    */
    int pos = xyq::get_prefix_by_separator(inbuf, prefix, sep, max_prefix_len);
    if (pos == -1)
    {
        return -1;
    }
    else
    {
        int rest_len = strlen(inbuf) - pos - strlen(sep);
        if (rest_len > max_rest_len - 1)
            throw std::out_of_range("char* rest buffer长度不足");
        strcpy(rest, inbuf + pos + strlen(sep));
        return 0;
    }
}
int xyq::get_prefix_by_separator(const char *inbuf, char *prefix, const char *sep, int max_prefix_len)
{
    /*
    @param:
        inbuf           需要解析的字符串
        prefix          前缀输出缓冲区
        sep             分隔序列
        max_prefix_len  前缀缓冲区长度
    @function:
        从字符串inbuf找到分隔符sep,并将sep之前的内容保存到prefix中
    @return
        int类型，表示sep出现的位置
        未找到sep则返回-1
    */
    int lensep = strlen(sep);
    if (lensep == 1)
        return get_prefix_by_separator(inbuf, prefix, sep[0], max_prefix_len);
    else
    {
        const char *pos = ::strstr(inbuf, sep);
        if (pos == nullptr)
        {
            return -1;
        }
        else
        {
            int ipos = pos - inbuf;
            int cnt = 0;
            if (ipos + 1 > max_prefix_len)
                throw std::out_of_range("char *prefix buffer长度不足！！");
            while (cnt < ipos)
            {
                prefix[cnt] = inbuf[cnt];
                cnt++;
            }
            prefix[cnt] = '\0';
        }
        return pos - inbuf;
    }
}
int xyq::get_prefix_by_separator(const char *inbuf, char *prefix, char sep, int max_prefix_len)
{
    int idx = 0;
    int inbuf_len = strlen(inbuf);

    for (idx = 0; idx < strlen(inbuf) and inbuf[idx] != sep; idx++)
    {
        if (max_prefix_len - 1 <= idx)
        {
            throw std::out_of_range("char *prefix buffer长度不足！！");
        }
        prefix[idx] = inbuf[idx];
    }
    if (idx >= strlen(inbuf))
    {
        return -1;
    }
    else
    {
        prefix[idx] = '\0';
        return idx;
    }
}

size_t xyq::get_file_size(const char *file_path)
{
    if (file_path == NULL)
        return 0;
    struct stat statbuf;
    if (stat(file_path, &statbuf) == 0)
        return statbuf.st_size;
    else
        return 0;
}

std::string xyq::get_time_now()
{
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto &&now_tm = std::localtime(&tt);
    char buf[12];
    memset(buf, 0, sizeof(buf));
    std::strftime(buf, sizeof(buf), "%H:%M:%S", now_tm);
    return buf;
}