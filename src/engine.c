#include "engine.h"
#include "common.h"

#include "display.h"
#include "gfx.h"
#include "input.h"

typedef struct {
	b8 running;
} engine_ctx_t;

static engine_ctx_t* g_engine = 0;

static mesh_t cube;

void engine_init() {
	g_engine = calloc(1, sizeof(engine_ctx_t));

	display_init();
	gfx_init();

	cube = gfx_load_mesh("untitled.obj");

	g_engine->running = TRUE;
}

void engine_fin() {
	gfx_fin();
	display_fin();

	free(g_engine);
}

void engine_run() {
	float i = 0.0f;
	mat4x4 viewProj;
	mat4x4_perspective(viewProj, 90.0f, 800.0f / 600.0f, 0.1f, 1000.0f);

	mat4x4 transform;
	mat4x4_identity(transform);

	while (g_engine->running) {
		display_poll_events();
		gfx_begin_frame();

		gfx_set_viewProjection(viewProj);
		gfx_draw_mesh(transform, &cube);

		gfx_end_frame();
		display_present();
	}
}

void engine_quit() {
	g_engine->running = FALSE;
}