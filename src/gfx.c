#include "gfx.h"

#include "shaders/sprite_batch.glsl"
#include "shaders/default.glsl"

#include <glad/glad.h>
#include <linmath/linmath.h>

typedef struct {
	GLuint vao,
		spriteProgram,
		spriteVertices,
		spriteIndices,
		stdProgram,
		mvpBuffer;

} gfx_ctx_t;

static gfx_ctx_t* g_gfx = 0;

typedef struct {
	vec3 position;
} vertex_t;

void gfx_init() {
	g_gfx = calloc(1, sizeof(gfx_ctx_t));

	gladLoadGL();
	
	glGenVertexArrays(1, &g_gfx->vao);
	glBindVertexArray(g_gfx->vao);
	
	g_gfx->spriteProgram = gfx_compile_shader(glsl_sprite_vs, glsl_sprite_fs);
	g_gfx->stdProgram = gfx_compile_shader(glsl_default_vs, glsl_default_fs);

	glGenBuffers(1, &g_gfx->mvpBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->mvpBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2, 0, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, g_gfx->mvpBuffer);

	glGenBuffers(1, &g_gfx->spriteVertices);
	glBindBuffer(GL_ARRAY_BUFFER, g_gfx->spriteVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, 0, GL_DYNAMIC_DRAW);

	static u32 indices[] = {
		0, 1, 2, 1, 2, 3
	};
	glGenBuffers(1, &g_gfx->spriteIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_gfx->spriteIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
}

void gfx_fin() {
	glDeleteBuffers(1, &g_gfx->mvpBuffer);
	glDeleteProgram(g_gfx->spriteProgram);
	glDeleteVertexArrays(1, &g_gfx->vao);

	free(g_gfx);
}

void gfx_begin_frame() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfx_end_frame() {

}

void gfx_set_viewProjection(mat4x4 viewProjection) {
	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->mvpBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4x4), viewProjection);
}

void gfx_draw_sprite(rect_t* rect) {
	glUseProgram(g_gfx->spriteProgram);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
	glEnableVertexAttribArray(0);
	vec2 vertices[] = {
		{ rect->x, rect->y },
		{ rect->x + rect->w, rect->y },
		{ rect->x, rect->y + rect->h},
		{ rect->x + rect->w, rect->y + rect->h }
	};

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gfx_draw_mesh(mat4x4 transform, mesh_t* mesh) {
	glUseProgram(g_gfx->stdProgram);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->mvpBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4),
		transform);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

	glDrawArrays(GL_TRIANGLES, 0, mesh->numElements);
}


u32 gfx_compile_shader(const char* vs_src, const char* fs_src) {
	char infoLog[512];
b8 success = FALSE;
u32 vs = glCreateShader(GL_VERTEX_SHADER),
fs = glCreateShader(GL_FRAGMENT_SHADER),
shaderStage = 0;

// compile vertex shader module
shaderStage = GL_VERTEX_SHADER;
glShaderSource(vs, 1, &vs_src, 0);
glCompileShader(vs);
glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
if (!success)
goto compile_fail;

// compile fragment shader module
shaderStage = GL_FRAGMENT_SHADER;
glShaderSource(fs, 1, &fs_src, 0);
glCompileShader(fs);
glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
if (!success)
goto compile_fail;

// link modules into shader program
u32 program = glCreateProgram();
glAttachShader(program, vs);
glAttachShader(program, fs);
glLinkProgram(program);
glGetProgramiv(program, GL_LINK_STATUS, &success);
if (!success)
goto link_fail;

// success
return program;

compile_fail:
glGetShaderInfoLog(
	(shaderStage == GL_VERTEX_SHADER) ? vs : fs,
	512, 0, infoLog);
printf("Failed to compile %s shader:\n%s\n",
	(shaderStage == GL_VERTEX_SHADER) ? "vertex" : "fragment", infoLog);
glDeleteShader(vs),
glDeleteShader(fs);

return 0;

link_fail:
glGetProgramInfoLog(program, 512, 0, infoLog);
printf("Failed to link shader:\n%s\n", infoLog);
glDeleteShader(vs);
glDeleteShader(fs);
glDeleteProgram(program);

return 0;
}




mesh_t gfx_load_mesh(const char* filepath) {
	mesh_t mesh;

	FILE* fp = fopen(filepath, "rb");
	if (!fp) {
		printf("Failed to load mesh: %s\n", filepath);
		return mesh;
	}

	// first pass: get allocation size info
	size_t numPositions = 0,
		numFaces = 0;

	char line[256];
	while (!feof(fp)) {
		fgets(line, 4, fp);

		if (strncmp(line, "v ", 2) == 0)
			numPositions++;
		if (strncmp(line, "f ", 2) == 0)
			numFaces++;
	}

	rewind(fp);

	// second pass: get the data
	vec3* positions = malloc(sizeof(vec3) * numPositions);
	u32 pi = 0;

	vec3 in_positions;
	u32 position_indices[3];

	vertex_t* vertices = malloc(sizeof(vertex_t) * numFaces * 3);
	u32 vertexIndex = 0;
	while (!feof(fp)) {
		fgets(line, 256, fp);

		if (sscanf(line, "v %f %f %f\n",
			&in_positions[0], &in_positions[1], &in_positions[2]) == 3) {
			memcpy(&positions[pi++], in_positions, sizeof(vec3));
		}

		if (sscanf(line, "f %i/%*i/%*i %i/%*i/%*i %i/%*i/%*i\n",
			&position_indices[0], &position_indices[1], &position_indices[2]) == 3) {
			vertex_t face[3];
			vec3_dup(face[0].position, positions[position_indices[0]]);
			vec3_dup(face[1].position, positions[position_indices[1]]);
			vec3_dup(face[2].position, positions[position_indices[2]]);

			memcpy(&vertices[vertexIndex], face, sizeof(face));
		}
	}

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * numFaces * 3, vertices,
		GL_STATIC_DRAW);

	free(positions);
	free(vertices);

	mesh.numElements = numFaces * 3;

	return mesh;
}