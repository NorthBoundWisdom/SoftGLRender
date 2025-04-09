/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#pragma once

#include <memory>
#include <vector>

#include "Base/GLMInc.h"

namespace SoftGL
{

class VertexArrayObject
{
public:
    virtual int getId() const = 0;
    virtual void updateVertexData(void *data, std::size_t length) = 0;
};

// only support float type attributes
struct VertexAttributeDesc
{
    std::size_t size;
    std::size_t stride;
    std::size_t offset;
};

struct VertexArray
{
    std::size_t vertexSize = 0;
    std::vector<VertexAttributeDesc> vertexesDesc;

    uint8_t *vertexesBuffer = nullptr;
    std::size_t vertexesBufferLength = 0;

    int32_t *indexBuffer = nullptr;
    std::size_t indexBufferLength = 0;
};

} // namespace SoftGL
