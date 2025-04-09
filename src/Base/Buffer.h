/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#ifndef SOFTGL_BUFFER_H
#define SOFTGL_BUFFER_H

#include "MemoryUtils.h"

namespace SoftGL
{
enum BufferLayout
{
    Layout_Linear, // 线性布局
    Layout_Tiled,  // 瓦片布局
    Layout_Morton, // 莫顿曲线布局
};

template <typename T>
class Buffer
{
public:
    virtual ~Buffer() = default;
    static std::shared_ptr<Buffer<T>> makeLayout(std::size_t w, std::size_t h, BufferLayout layout);

    virtual void initLayout()
    {
        innerWidth_ = width_;
        innerHeight_ = height_;
    }

    // 将二维坐标转换为一维数组索引
    virtual inline std::size_t convertIndex(std::size_t x, std::size_t y) const
    {
        return x + y * innerWidth_;
    }

    // 获取当前缓冲区的布局类型
    virtual BufferLayout getLayout() const
    {
        return Layout_Linear;
    }

    // 创建缓冲区，可选择提供初始数据
    void create(std::size_t w, std::size_t h, const uint8_t *data = nullptr)
    {
        if (w > 0 && h > 0)
        {
            if (width_ == w && height_ == h)
            {
                return;
            }
            width_ = w;
            height_ = h;

            initLayout();
            dataSize_ = innerWidth_ * innerHeight_;
            data_ = MemoryUtils::makeBuffer<T>(dataSize_, data);
        }
    }

    // 销毁缓冲区，释放资源
    virtual void destroy()
    {
        width_ = 0;
        height_ = 0;
        innerWidth_ = 0;
        innerHeight_ = 0;
        dataSize_ = 0;
        data_ = nullptr;
    }

    // 获取原始数据指针
    inline T *getRawDataPtr() const
    {
        return data_.get();
    }

    // 获取原始数据大小（元素数量）
    inline std::size_t getRawDataSize() const
    {
        return dataSize_;
    }

    // 获取原始数据字节大小
    inline std::size_t getRawDataBytesSize() const
    {
        return dataSize_ * sizeof(T);
    }

    // 检查缓冲区是否为空
    inline bool empty() const
    {
        return data_ == nullptr;
    }

    // 获取缓冲区宽度
    inline std::size_t getWidth() const
    {
        return width_;
    }

    // 获取缓冲区高度
    inline std::size_t getHeight() const
    {
        return height_;
    }

    // 获取指定坐标的元素指针
    inline T *get(std::size_t x, std::size_t y)
    {
        T *ptr = data_.get();
        if (ptr != nullptr && x < width_ && y < height_)
        {
            return &ptr[convertIndex(x, y)];
        }
        return nullptr;
    }

    // 设置指定坐标的元素值
    inline void set(std::size_t x, std::size_t y, const T &pixel)
    {
        T *ptr = data_.get();
        if (ptr != nullptr && x < width_ && y < height_)
        {
            ptr[convertIndex(x, y)] = pixel;
        }
    }

    // 将原始数据复制到目标内存，可选择是否垂直翻转
    void copyRawDataTo(T *out, bool flip_y = false) const
    {
        T *ptr = data_.get();
        if (ptr != nullptr)
        {
            if (!flip_y)
            {
                memcpy(out, ptr, dataSize_ * sizeof(T));
            }
            else
            {
                for (int i = 0; i < innerHeight_; i++)
                {
                    memcpy(out + innerWidth_ * i, ptr + innerWidth_ * (innerHeight_ - 1 - i),
                           innerWidth_ * sizeof(T));
                }
            }
        }
    }

    // 清空缓冲区（置零）
    inline void clear() const
    {
        T *ptr = data_.get();
        if (ptr != nullptr)
        {
            memset(ptr, 0, dataSize_ * sizeof(T));
        }
    }

    // 设置所有元素为指定值
    inline void setAll(T val) const
    {
        T *ptr = data_.get();
        if (ptr != nullptr)
        {
            for (int i = 0; i < dataSize_; i++)
            {
                ptr[i] = val;
            }
        }
    }

protected:
    std::size_t width_ = 0;             // 缓冲区宽度
    std::size_t height_ = 0;            // 缓冲区高度
    std::size_t innerWidth_ = 0;        // 内部实际宽度（可能因布局调整而不同于width_）
    std::size_t innerHeight_ = 0;       // 内部实际高度（可能因布局调整而不同于height_）
    std::shared_ptr<T> data_ = nullptr; // 数据存储指针
    std::size_t dataSize_ = 0;          // 数据元素总数
};

template <typename T>
class TiledBuffer : public Buffer<T>
{
public:
    // 初始化瓦片布局，计算瓦片数量和内部尺寸
    void initLayout() override
    {
        tileWidth_ = (this->width_ + tileSize_ - 1) / tileSize_;
        tileHeight_ = (this->height_ + tileSize_ - 1) / tileSize_;
        this->innerWidth_ = tileWidth_ * tileSize_;
        this->innerHeight_ = tileHeight_ * tileSize_;
    }

    // 将二维坐标转换为瓦片布局中的一维索引
    inline std::size_t convertIndex(std::size_t x, std::size_t y) const override
    {
        std::size_t tileX = x >> bits_;            // x / tileSize_，计算瓦片X坐标
        std::size_t tileY = y >> bits_;            // y / tileSize_，计算瓦片Y坐标
        std::size_t inTileX = x & (tileSize_ - 1); // x % tileSize_，计算瓦片内X坐标
        std::size_t inTileY = y & (tileSize_ - 1); // y % tileSize_，计算瓦片内Y坐标

        return ((tileY * tileWidth_ + tileX) << bits_ << bits_) + (inTileY << bits_) + inTileX;
    }

    // 获取布局类型为瓦片布局
    BufferLayout getLayout() const override
    {
        return Layout_Tiled;
    }

private:
    const static int tileSize_ = 4; // 瓦片大小 4 x 4
    const static int bits_ = 2;     // tileSize_ = 2^bits_，用于位运算优化
    std::size_t tileWidth_ = 0;     // 横向瓦片数量
    std::size_t tileHeight_ = 0;    // 纵向瓦片数量
};

template <typename T>
class MortonBuffer : public Buffer<T>
{
public:
    // 初始化莫顿曲线布局，计算瓦片数量和内部尺寸
    void initLayout() override
    {
        tileWidth_ = (this->width_ + tileSize_ - 1) / tileSize_;
        tileHeight_ = (this->height_ + tileSize_ - 1) / tileSize_;
        this->innerWidth_ = tileWidth_ * tileSize_;
        this->innerHeight_ = tileHeight_ * tileSize_;
    }

    /**
     * 编码16位莫顿码
     * Ref: https://gist.github.com/JarkkoPFC/0e4e599320b0cc7ea92df45fb416d79a
     */
    static inline uint16_t encode16_morton2(uint8_t x_, uint8_t y_)
    {
        uint32_t res = x_ | (uint32_t(y_) << 16);
        res = (res | (res << 4)) & 0x0f0f0f0f;
        res = (res | (res << 2)) & 0x33333333;
        res = (res | (res << 1)) & 0x55555555;
        return uint16_t(res | (res >> 15));
    }

    // 将二维坐标转换为莫顿曲线布局中的一维索引
    inline std::size_t convertIndex(std::size_t x, std::size_t y) const override
    {
        std::size_t tileX = x >> bits_;            // x / tileSize_，计算瓦片X坐标
        std::size_t tileY = y >> bits_;            // y / tileSize_，计算瓦片Y坐标
        std::size_t inTileX = x & (tileSize_ - 1); // x % tileSize_，计算瓦片内X坐标
        std::size_t inTileY = y & (tileSize_ - 1); // y % tileSize_，计算瓦片内Y坐标

        std::size_t mortonIndex = encode16_morton2(inTileX, inTileY);

        return ((tileY * tileWidth_ + tileX) << bits_ << bits_) + mortonIndex;
    }

    // 获取布局类型为莫顿曲线布局
    BufferLayout getLayout() const override
    {
        return Layout_Morton;
    }

private:
    const static int tileSize_ = 32; // 瓦片大小 32 x 32
    const static int bits_ = 5;      // tileSize_ = 2^bits_，用于位运算优化
    std::size_t tileWidth_ = 0;      // 横向瓦片数量
    std::size_t tileHeight_ = 0;     // 纵向瓦片数量
};

template <typename T>
std::shared_ptr<Buffer<T>> Buffer<T>::makeLayout(std::size_t w, std::size_t h, BufferLayout layout)
{
    std::shared_ptr<Buffer<T>> ret = nullptr;
    switch (layout)
    {
    case Layout_Tiled:
    {
        ret = std::make_shared<TiledBuffer<T>>();
    }
    case Layout_Morton:
    {
        ret = std::make_shared<MortonBuffer<T>>();
    }
    case Layout_Linear:
    {
        ret = std::make_shared<Buffer<T>>();
    }
    }

    ret->create(w, h);
    return ret;
}
} // namespace SoftGL

#endif // SOFTGL_BUFFER_H
