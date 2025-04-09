/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */
#ifndef GLM_INC_H
#define GLM_INC_H

#define GLM_FORCE_ALIGNED

#ifdef MSVC_COMPILER
#define GLM_FORCE_INLINE
#define GLM_FORCE_AVX2
#endif

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_aligned.hpp>

using RGBA = glm::u8vec4;

#endif // GLM_INC_H
