/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */
#ifndef SOFTGL_FILE_UTILS_H
#define SOFTGL_FILE_UTILS_H

#include <fstream>
#include <ios>
#include <vector>

#include "Logger.h"

namespace SoftGL
{
class FileUtils
{
public:
    static bool exists(const std::string &path)
    {
        std::ifstream file(path);
        return file.good();
    }

    static std::vector<uint8_t> readBytes(const std::string &path)
    {
        std::vector<uint8_t> ret;
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            LOGE("failed to open file: %s", path.c_str());
            return ret;
        }

        std::streamoff size = file.tellg();
        if (size <= 0)
        {
            LOGE("failed to read file, invalid size: %d", size);
            return ret;
        }

        ret.resize(static_cast<std::size_t>(size));

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(ret.data()), (std::streamsize)size);
        file.close();
        return ret;
    }

    static std::string readText(const std::string &path)
    {
        auto data = readBytes(path);
        if (data.empty())
        {
            return "";
        }

        return {(char *)data.data(), data.size()};
    }

    static bool writeBytes(const std::string &path, const char *data, std::streamsize length)
    {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (!file.is_open())
        {
            LOGE("failed to open file: %s", path.c_str());
            return false;
        }

        file.write(data, length);
        file.close();
        return true;
    }

    static bool writeText(const std::string &path, const std::string &str)
    {
        std::ofstream file(path, std::ios::out);
        if (!file.is_open())
        {
            LOGE("failed to open file: %s", path.c_str());
            return false;
        }

        file.write(str.c_str(), static_cast<std::streamsize>(str.length()));
        file.close();

        return true;
    }
};
} // namespace SoftGL
#endif // SOFTGL_FILE_UTILS_H
