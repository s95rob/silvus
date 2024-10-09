#pragma once

#include "common.h"

#include <linmath/linmath.h>

typedef struct {
	f32 x, y, w, h;
} rect_t;

typedef struct {
	u32 vbo, ibo,
		numElements;
} mesh_t;

void gfx_init();
void gfx_fin();

void gfx_begin_frame();
void gfx_end_frame();

void gfx_set_viewProjection(mat4x4 viewProjection);

void gfx_draw_sprite(rect_t* rect);
void gfx_draw_mesh(mat4x4 transform, mesh_t* mesh);

mesh_t gfx_load_mesh(const char* filepath);
void gfx_free_mesh(mesh_t* mesh);