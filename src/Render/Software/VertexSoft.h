/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#pragma once

#include "Base/UUID.h"
#include "Render/Vertex.h"

namespace SoftGL
{

class VertexArrayObjectSoft : public VertexArrayObject
{
public:
    explicit VertexArrayObjectSoft(const VertexArray &vertexArray)
    {
        // init vertexes
        vertexStride = vertexArray.vertexesDesc[0].stride;
        vertexCnt = vertexArray.vertexesBufferLength / vertexStride;
        vertexes.resize(vertexCnt * vertexStride);
        memcpy(vertexes.data(), vertexArray.vertexesBuffer, vertexArray.vertexesBufferLength);

        // init indices
        indicesCnt = vertexArray.indexBufferLength / sizeof(int32_t);
        indices.resize(indicesCnt);
        memcpy(indices.data(), vertexArray.indexBuffer, vertexArray.indexBufferLength);
    }

    void updateVertexData(void *data, std::size_t length) override
    {
        memcpy(vertexes.data(), data, std::min(length, vertexes.size()));
    }

    int getId() const override
    {
        return uuid_.get();
    }

public:
    std::size_t vertexStride = 0;
    std::size_t vertexCnt = 0;
    std::size_t indicesCnt = 0;
    std::vector<uint8_t> vertexes;
    std::vector<int32_t> indices;

private:
    UUID<VertexArrayObjectSoft> uuid_;
};

} // namespace SoftGL
