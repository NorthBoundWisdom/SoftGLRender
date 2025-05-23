/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#pragma once

#include <functional>

#include "Camera.h"
#include "Model.h"
#include "Render/Framebuffer.h"
#include "Render/Renderer.h"
#include "Render/Texture.h"

namespace SoftGL
{
namespace View
{

constexpr int kIrradianceMapSize = 32;
constexpr int kPrefilterMaxMipLevels = 5;
constexpr int kPrefilterMapSize = 128;

struct CubeRenderContext
{
    std::shared_ptr<FrameBuffer> fbo;
    Camera camera;
    ModelMesh modelSkybox;
    std::shared_ptr<UniformBlock> uniformsBlockModel;
};

class IBLGenerator
{
public:
    explicit IBLGenerator(const std::shared_ptr<Renderer> &renderer)
        : renderer_(renderer) {};
    inline void clearCaches()
    {
        contextCache_.clear();
    }

    bool convertEquirectangular(const std::function<bool(ShaderProgram &program)> &shaderFunc,
                                const std::shared_ptr<Texture> &texIn,
                                std::shared_ptr<Texture> &texOut);

    bool generateIrradianceMap(const std::function<bool(ShaderProgram &program)> &shaderFunc,
                               const std::shared_ptr<Texture> &texIn,
                               std::shared_ptr<Texture> &texOut);

    bool generatePrefilterMap(const std::function<bool(ShaderProgram &program)> &shaderFunc,
                              const std::shared_ptr<Texture> &texIn,
                              std::shared_ptr<Texture> &texOut);

private:
    bool createCubeRenderContext(CubeRenderContext &context,
                                 const std::function<bool(ShaderProgram &program)> &shaderFunc,
                                 const std::shared_ptr<Texture> &texIn, MaterialTexType texType);

    void drawCubeFaces(CubeRenderContext &context, uint32_t width, uint32_t height,
                       std::shared_ptr<Texture> &texOut, uint32_t texOutLevel = 0,
                       const std::function<void()> &beforeDraw = nullptr);

    bool loadFromCache(std::shared_ptr<Texture> &tex);
    void storeToCache(std::shared_ptr<Texture> &tex);

    static std::string getTextureHashKey(std::shared_ptr<Texture> &tex);
    static std::string getCacheFilePath(const std::string &hashKey);

private:
    std::shared_ptr<Renderer> renderer_;
    std::vector<std::shared_ptr<CubeRenderContext>> contextCache_;
};

} // namespace View
} // namespace SoftGL
