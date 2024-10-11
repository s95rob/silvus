#pragma once

// signed 8bit value
typedef char s8;

// signed 16bit value
typedef short s16;

// signed 32bit value
typedef int s32;

// unsigned 8bit value
typedef unsigned char u8;

// unsigned 16bit value
typedef unsigned short u16;

// unsigned 32bit value
typedef unsigned int u32;

// signed 32bit float
typedef float f32;

// signed 32bit bool
typedef int b8;

#define TRUE 1
#define FALSE 0

#include <malloc.h>
#include <stdio.h>

#include <linmath/linmath.h>

// gfx/physics things

#pragma push(pack, 16)

typedef struct {
	vec3 diffuse;
	u32 diffuse_tex;
} material_t;

#pragma pop

typedef struct {
	f32 x, y, w, h;
} rect_t;

typedef struct {
	vec3 pos, norm;
} vertex_t;

typedef vertex_t face_t[3];

typedef struct {
	u32 vbo,
		uvnbo,
		numFaces;
	face_t* faces;
	material_t material;
} mesh_t;