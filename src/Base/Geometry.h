/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */
#ifndef SOFTGL_GEOMETRY_H
#define SOFTGL_GEOMETRY_H

#include <array>

#include "Base/GLMInc.h"

namespace SoftGL
{
class BoundingBox
{
public:
    BoundingBox() = default;
    BoundingBox(const glm::vec3 &a, const glm::vec3 &b)
        : min(a)
        , max(b)
    {
    }

    void getCorners(std::array<glm::vec3, 8> &dst) const;
    BoundingBox transform(const glm::mat4 &matrix) const;
    bool intersects(const BoundingBox &box) const;
    void merge(const BoundingBox &box);

protected:
    static void updateMinMax(glm::vec3 *point, glm::vec3 *min, glm::vec3 *max);

public:
    glm::vec3 min{0.f, 0.f, 0.f};
    glm::vec3 max{0.f, 0.f, 0.f};
};

class Plane
{
public:
    enum PlaneIntersects
    {
        Intersects_Cross = 0,
        Intersects_Tangent = 1,
        Intersects_Front = 2,
        Intersects_Back = 3
    };

    void set(const glm::vec3 &n, const glm::vec3 &pt)
    {
        normal_ = glm::normalize(n);
        d_ = -(glm::dot(normal_, pt));
    }

    float distance(const glm::vec3 &pt) const
    {
        return glm::dot(normal_, pt) + d_;
    }

    inline const glm::vec3 &getNormal() const
    {
        return normal_;
    }

    PlaneIntersects intersects(const BoundingBox &box) const;

    // check intersect with point (world space)
    PlaneIntersects intersects(const glm::vec3 &p0) const;

    // check intersect with line segment (world space)
    PlaneIntersects intersects(const glm::vec3 &p0, const glm::vec3 &p1) const;

    // check intersect with triangle (world space)
    PlaneIntersects intersects(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) const;

private:
    glm::vec3 normal_;
    float d_ = 0;
};

struct Frustum
{
public:
    bool intersects(const BoundingBox &box) const;

    // check intersect with point (world space)
    bool intersects(const glm::vec3 &p0) const;

    // check intersect with line segment (world space)
    bool intersects(const glm::vec3 &p0, const glm::vec3 &p1) const;

    // check intersect with triangle (world space)
    bool intersects(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) const;

public:
    /**
     * planes[0]: near;
     * planes[1]: far;
     * planes[2]: top;
     * planes[3]: bottom;
     * planes[4]: left;
     * planes[5]: right;
     */
    std::array<Plane, 6> planes;

    /**
     * corners[0]: nearTopLeft;
     * corners[1]: nearTopRight;
     * corners[2]: nearBottomLeft;
     * corners[3]: nearBottomRight;
     * corners[4]: farTopLeft;
     * corners[5]: farTopRight;
     * corners[6]: farBottomLeft;
     * corners[7]: farBottomRight;
     */
    std::array<glm::vec3, 8> corners;

    BoundingBox bbox;
};

enum FrustumClipMask
{
    POSITIVE_X = 1 << 0,
    NEGATIVE_X = 1 << 1,
    POSITIVE_Y = 1 << 2,
    NEGATIVE_Y = 1 << 3,
    POSITIVE_Z = 1 << 4,
    NEGATIVE_Z = 1 << 5,
};

const int FrustumClipMaskArray[6] = {
    FrustumClipMask::POSITIVE_X, FrustumClipMask::NEGATIVE_X, FrustumClipMask::POSITIVE_Y,
    FrustumClipMask::NEGATIVE_Y, FrustumClipMask::POSITIVE_Z, FrustumClipMask::NEGATIVE_Z,
};

const glm::vec4 FrustumClipPlane[6] = {{-1, 0, 0, 1}, {1, 0, 0, 1},  {0, -1, 0, 1},
                                       {0, 1, 0, 1},  {0, 0, -1, 1}, {0, 0, 1, 1}};

} // namespace SoftGL
#endif // SOFTGL_GEOMETRY_H
