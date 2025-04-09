/*
 * SoftGLRender
 * @author 	: keith@robot9.me
 *
 */

#pragma once

#ifdef MSVC_COMPILER
#define MM_F32(v, i) v.m128_f32[i]
#else
#define MM_F32(v, i) v[i]
#endif

#define PTR_ADDR(p) ((std::size_t)(p))
