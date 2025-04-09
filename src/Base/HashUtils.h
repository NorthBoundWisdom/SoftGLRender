/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <array>
#include <string>

#include <md5/md5.h>

namespace SoftGL
{
/**
 * @brief 哈希工具类，提供多种哈希算法和哈希组合方法
 */
class HashUtils
{
public:
    /**
     * @brief 将单个值合并到现有的哈希种子中
     *
     * 使用 FNV-1a 哈希算法变体将单个值合并到现有的哈希种子中
     * 适用于需要组合多个值生成哈希的场景
     *
     * @tparam T 要哈希的值类型
     * @param seed 现有的哈希种子，将被更新
     * @param v 要合并到哈希中的值
     */
    template <class T>
    inline static void hashCombine(std::size_t &seed, T const &v)
    {
        seed ^= std::hash<T>()(v) + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    }

    /**
     * @brief 计算给定数据的 MurmurHash3 32位哈希值
     *
     * MurmurHash3 是一种非加密哈希函数，具有高速度和良好的分布特性
     *
     * @param key 要哈希的数据指针
     * @param wordCount 要哈希的32位字数量
     * @param seed 哈希种子
     * @return 计算得到的32位哈希值
     */
    inline static uint32_t murmur3(const uint32_t *key, std::size_t wordCount,
                                   uint32_t seed) noexcept
    {
        uint32_t h = seed;
        std::size_t i = wordCount;
        do
        {
            uint32_t k = *key++;
            k *= 0xcc9e2d51u;
            k = (k << 15u) | (k >> 17u);
            k *= 0x1b873593u;
            h ^= k;
            h = (h << 13u) | (h >> 19u);
            h = (h * 5u) + 0xe6546b64u;
        } while (--i);
        h ^= wordCount;
        h ^= h >> 16u;
        h *= 0x85ebca6bu;
        h ^= h >> 13u;
        h *= 0xc2b2ae35u;
        h ^= h >> 16u;
        return h;
    }

    /**
     * @brief 使用 MurmurHash3 算法将对象哈希值合并到现有种子中
     *
     * 将对象转换为字节数组，然后使用 MurmurHash3 计算哈希值并合并到种子中
     *
     * @tparam T 要哈希的对象类型
     * @param seed 现有的哈希种子，将被更新
     * @param key 要哈希的对象
     */
    template <typename T>
    inline static void hashCombineMurmur(std::size_t &seed, const T &key)
    {
        static_assert(0 == (sizeof(key) & 3u), "Hashing requires a size that is a multiple of 4.");
        uint32_t keyHash = HashUtils::murmur3((const uint32_t *)&key, sizeof(key) / 4, 0);
        seed ^= keyHash + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
    }

    /**
     * @brief 计算给定数据的 MD5 哈希值并返回十六进制字符串
     *
     * @param data 要哈希的数据指针
     * @param length 数据长度
     * @return 32字符的十六进制 MD5 哈希字符串
     */
    inline static std::string getHashMD5(const char *data, std::size_t length)
    {
        std::array<unsigned char, 17> digest = {0};

        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data)),
                   static_cast<unsigned int>(length));
        MD5_Final(digest.data(), &ctx);

        std::array<char, 33> str = {0};
        hexToStr(str, digest, 16);
        return {str.data()};
    }

    /**
     * @brief 计算字符串的 MD5 哈希值并返回十六进制字符串
     *
     * @param text 要哈希的字符串
     * @return 32字符的十六进制 MD5 哈希字符串
     */
    inline static std::string getHashMD5(const std::string &text)
    {
        return getHashMD5(text.c_str(), text.length());
    }

private:
    /**
     * @brief 将二进制数据转换为十六进制字符串
     *
     * @param str 输出字符串缓冲区
     * @param digest 二进制数据
     * @param length 数据长度
     */
    static void hexToStr(std::array<char, 33> &str, const std::array<unsigned char, 17> &digest,
                         std::size_t length)
    {
        uint8_t hexDigit;
        for (std::size_t i = 0; i < length; i++)
        {
            hexDigit = (digest[i] >> 4) & 0xF;
            str[i * 2] = (hexDigit <= 9) ? (hexDigit + '0') : (hexDigit + 'a' - 10);
            hexDigit = digest[i] & 0xF;
            str[i * 2 + 1] = (hexDigit <= 9) ? (hexDigit + '0') : (hexDigit + 'a' - 10);
        }
    }
};
} // namespace SoftGL

#endif // HASH_UTILS_H
