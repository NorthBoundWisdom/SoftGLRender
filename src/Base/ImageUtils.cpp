/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#include "ImageUtils.h"

#include <span>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <algorithm>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "Logger.h"

namespace SoftGL
{
/**
 * @brief 读取图像文件并转换为RGBA格式的缓冲区
 *
 * 使用stb_image库读取图像文件，支持多种格式（灰度、灰度+alpha、RGB、RGBA）
 * 并将图像数据转换为统一的RGBA格式存储在缓冲区中
 *
 * @param path 图像文件路径
 * @return 包含RGBA图像数据的缓冲区，如果读取失败则返回nullptr
 */
std::shared_ptr<Buffer<RGBA>> ImageUtils::readImageRGBA(const std::string &path)
{
    int iw = 0, ih = 0, n = 0;

    unsigned char *data = stbi_load(path.c_str(), &iw, &ih, &n, STBI_default);
    if (data == nullptr)
    {
        LOGD("ImageUtils::readImage failed, path: %s", path.c_str());
        return nullptr;
    }

    // put data into
    std::size_t size_t_iw = static_cast<std::size_t>(iw);
    std::size_t size_t_ih = static_cast<std::size_t>(ih);
    std::size_t size_t_n = static_cast<std::size_t>(n);
    std::span<unsigned char> data_span(data, size_t_iw * size_t_ih * size_t_n);
    auto buffer = Buffer<RGBA>::makeLayout(size_t_iw, size_t_ih, BufferLayout::Layout_Linear);

    // convert to rgba
    for (std::size_t y = 0; y < size_t_ih; y++)
    {
        for (std::size_t x = 0; x < size_t_iw; x++)
        {
            auto &to = *buffer->get(x, y);
            std::size_t idx = x + y * size_t_iw;

            switch (n)
            {
            case STBI_grey:
            {
                to.r = data_span[idx];
                to.g = to.b = to.r;
                to.a = 255;
                break;
            }
            case STBI_grey_alpha:
            {
                to.r = data_span[idx * 2 + 0];
                to.g = to.b = to.r;
                to.a = data_span[idx * 2 + 1];
                break;
            }
            case STBI_rgb:
            {
                to.r = data_span[idx * 3 + 0];
                to.g = data_span[idx * 3 + 1];
                to.b = data_span[idx * 3 + 2];
                to.a = 255;
                break;
            }
            case STBI_rgb_alpha:
            {
                to.r = data_span[idx * 4 + 0];
                to.g = data_span[idx * 4 + 1];
                to.b = data_span[idx * 4 + 2];
                to.a = data_span[idx * 4 + 3];
                break;
            }
            default: break;
            }
        }
    }

    stbi_image_free(data);

    return buffer;
}

/**
 * @brief 将图像数据写入文件
 *
 * 使用stb_image_write库将图像数据写入PNG格式文件
 *
 * @param filename 输出文件路径
 * @param w 图像宽度
 * @param h 图像高度
 * @param comp 每个像素的通道数（1=灰度，2=灰度+alpha，3=RGB，4=RGBA）
 * @param data 图像数据指针
 * @param strideInBytes 每行像素的字节数（用于处理填充）
 * @param flipY 是否垂直翻转图像
 */
void ImageUtils::writeImage(char const *filename, int w, int h, int comp, const void *data,
                            int strideInBytes, bool flipY)
{
    stbi_flip_vertically_on_write(flipY);
    stbi_write_png(filename, w, h, comp, data, strideInBytes);
}

/**
 * @brief 将浮点深度图转换为RGBA图像
 *
 * 将浮点深度值归一化到[0,1]范围，然后映射到[0,255]的灰度值
 * 结果存储在RGBA缓冲区中，所有颜色通道使用相同的灰度值
 *
 * @param dst 目标RGBA缓冲区
 * @param src 源浮点深度数据
 * @param width 图像宽度
 * @param height 图像高度
 */
void ImageUtils::convertFloatImage(RGBA *dst, float *src, uint32_t width, uint32_t height)
{
    float *srcPixel = src;

    float depthMin = FLT_MAX;
    float depthMax = FLT_MIN;
    for (uint32_t i = 0; i < width * height; i++)
    {
        float depth = *srcPixel;
        depthMin = std::min(depthMin, depth);
        depthMax = std::max(depthMax, depth);
        srcPixel++;
    }

    srcPixel = src;
    RGBA *dstPixel = dst;
    for (uint32_t i = 0; i < width * height; i++)
    {
        float depth = *srcPixel;
        depth = (depth - depthMin) / (depthMax - depthMin);
        int depth_int = static_cast<int>(depth * 255.f);
        dstPixel->r = static_cast<unsigned char>(glm::clamp(depth_int, 0, 255));
        dstPixel->g = dstPixel->r;
        dstPixel->b = dstPixel->r;
        dstPixel->a = 255;

        srcPixel++;
        dstPixel++;
    }
}
} // namespace SoftGL
