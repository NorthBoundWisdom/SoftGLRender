/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#include "Geometry.h"

#include <algorithm>

#include "Base/AuxMath.h"

namespace SoftGL
{

void BoundingBox::getCorners(std::array<glm::vec3, 8> &dst) const
{
    dst[0] = glm::vec3(min.x, max.y, max.z);
    dst[1] = glm::vec3(min.x, min.y, max.z);
    dst[2] = glm::vec3(max.x, min.y, max.z);
    dst[3] = glm::vec3(max.x, max.y, max.z);

    dst[4] = glm::vec3(max.x, max.y, min.z);
    dst[5] = glm::vec3(max.x, min.y, min.z);
    dst[6] = glm::vec3(min.x, min.y, min.z);
    dst[7] = glm::vec3(min.x, max.y, min.z);
}

void BoundingBox::updateMinMax(glm::vec3 *point, glm::vec3 *min, glm::vec3 *max)
{
    if (point->x < min->x)
    {
        min->x = point->x;
    }

    if (point->x > max->x)
    {
        max->x = point->x;
    }

    if (point->y < min->y)
    {
        min->y = point->y;
    }

    if (point->y > max->y)
    {
        max->y = point->y;
    }

    if (point->z < min->z)
    {
        min->z = point->z;
    }

    if (point->z > max->z)
    {
        max->z = point->z;
    }
}

BoundingBox BoundingBox::transform(const glm::mat4 &matrix) const
{
    std::array<glm::vec3, 8> corners;
    getCorners(corners);

    corners[0] = matrix * glm::vec4(corners[0], 1.f);
    glm::vec3 newMin = corners[0];
    glm::vec3 newMax = corners[0];
    for (std::size_t i = 1; i < 8; i++)
    {
        corners[i] = matrix * glm::vec4(corners[i], 1.f);
        updateMinMax(&corners[i], &newMin, &newMax);
    }
    return {newMin, newMax};
}

bool BoundingBox::intersects(const BoundingBox &box) const
{
    return ((min.x >= box.min.x && min.x <= box.max.x) ||
            (box.min.x >= min.x && box.min.x <= max.x)) &&
           ((min.y >= box.min.y && min.y <= box.max.y) ||
            (box.min.y >= min.y && box.min.y <= max.y)) &&
           ((min.z >= box.min.z && min.z <= box.max.z) ||
            (box.min.z >= min.z && box.min.z <= max.z));
}

void BoundingBox::merge(const BoundingBox &box)
{
    min.x = std::min(min.x, box.min.x);
    min.y = std::min(min.y, box.min.y);
    min.z = std::min(min.z, box.min.z);

    max.x = std::max(max.x, box.max.x);
    max.y = std::max(max.y, box.max.y);
    max.z = std::max(max.z, box.max.z);
}

Plane::PlaneIntersects Plane::intersects(const BoundingBox &box) const
{
    glm::vec3 center = (box.min + box.max) * 0.5f;
    glm::vec3 extent = (box.max - box.min) * 0.5f;
    float d = distance(center);
    float r = std::abs(extent.x * normal_.x) + std::abs(extent.y * normal_.y) +
              std::abs(extent.z * normal_.z);
    if (absEqual(d, r))
    {
        return PlaneIntersects::Intersects_Tangent;
    }
    else if (lessThan(std::abs(d), r))
    {
        return PlaneIntersects::Intersects_Cross;
    }

    return (d > 0.0f) ? PlaneIntersects::Intersects_Front : PlaneIntersects::Intersects_Back;
}

Plane::PlaneIntersects Plane::intersects(const glm::vec3 &p0) const
{
    float d = distance(p0);
    if (absEqual(d, 0.0f))
    {
        return PlaneIntersects::Intersects_Tangent;
    }
    return (d > 0.0f) ? PlaneIntersects::Intersects_Front : PlaneIntersects ::Intersects_Back;
}

Plane::PlaneIntersects Plane::intersects(const glm::vec3 &p0, const glm::vec3 &p1) const
{
    PlaneIntersects state0 = intersects(p0);
    PlaneIntersects state1 = intersects(p1);

    if (state0 == state1)
    {
        return state0;
    }

    if (state0 == PlaneIntersects::Intersects_Tangent ||
        state1 == PlaneIntersects::Intersects_Tangent)
    {
        return PlaneIntersects::Intersects_Tangent;
    }

    return PlaneIntersects::Intersects_Cross;
}

Plane::PlaneIntersects Plane::intersects(const glm::vec3 &p0, const glm::vec3 &p1,
                                         const glm::vec3 &p2) const
{
    PlaneIntersects state0 = intersects(p0, p1);
    PlaneIntersects state1 = intersects(p0, p2);
    PlaneIntersects state2 = intersects(p1, p2);

    if (state0 == state1 && state0 == state2)
    {
        return state0;
    }

    if (state0 == PlaneIntersects::Intersects_Cross ||
        state1 == PlaneIntersects::Intersects_Cross || state2 == PlaneIntersects::Intersects_Cross)
    {
        return PlaneIntersects::Intersects_Cross;
    }

    return PlaneIntersects::Intersects_Tangent;
}

bool Frustum::intersects(const BoundingBox &box) const
{
    for (auto &plane : planes)
    {
        if (plane.intersects(box) == Plane::Intersects_Back)
        {
            return false;
        }
    }

    // check box intersects
    if (!bbox.intersects(box))
    {
        return false;
    }

    return true;
}

bool Frustum::intersects(const glm::vec3 &p0) const
{
    for (auto &plane : planes)
    {
        if (plane.intersects(p0) == Plane::Intersects_Back)
        {
            return false;
        }
    }

    return true;
}

bool Frustum::intersects(const glm::vec3 &p0, const glm::vec3 &p1) const
{
    for (auto &plane : planes)
    {
        if (plane.intersects(p0, p1) == Plane::Intersects_Back)
        {
            return false;
        }
    }

    return true;
}

bool Frustum::intersects(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) const
{
    for (auto &plane : planes)
    {
        if (plane.intersects(p0, p1, p2) == Plane::Intersects_Back)
        {
            return false;
        }
    }

    return true;
}
} // namespace SoftGL
