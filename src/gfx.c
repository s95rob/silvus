#include "gfx.h"

#include "shaders/sprite_batch.glsl"
#include "shaders/default.glsl"

#include <glad/glad.h>
#include <linmath/linmath.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

typedef struct {
	GLuint vao,
		spriteProgram,
		spriteVertices,
		spriteIndices,
		stdProgram,
		mvpBuffer,
		matBuffer;

} gfx_ctx_t;

static gfx_ctx_t* g_gfx = 0;


void gfx_init() {
	g_gfx = calloc(1, sizeof(gfx_ctx_t));

	gladLoadGL();

	glEnable(GL_DEPTH);
	glEnable(GL_BLEND);
	
	glGenVertexArrays(1, &g_gfx->vao);
	glBindVertexArray(g_gfx->vao);
	
	g_gfx->spriteProgram = gfx_compile_shader(glsl_sprite_vs, glsl_sprite_fs);
	g_gfx->stdProgram = gfx_compile_shader(glsl_default_vs, glsl_default_fs);

	glGenBuffers(1, &g_gfx->mvpBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->mvpBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2, 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, g_gfx->mvpBuffer);

	glGenBuffers(1, &g_gfx->matBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->matBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(material_t), 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, g_gfx->matBuffer);

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

	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->mvpBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4),
		transform);
	glBindBuffer(GL_UNIFORM_BUFFER, g_gfx->matBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(material_t), &mesh->material);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, (void*)sizeof(vec3));
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->material.diffuse_tex);

	glDrawArrays(GL_TRIANGLES, 0, mesh->numFaces * 3);
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
	printf("Loading mesh \"%s\" ...\n", filepath);

	mesh_t mesh;
	memset(&mesh, 0, sizeof(mesh_t));
	memset(&mesh.material, 1.0f, sizeof(material_t));

	FILE* fpObj = fopen(filepath, "rb"),
		* fpMat = 0;

	if (!fpObj) {
		printf("Failed to load mesh: %s\n", filepath);
		return mesh;
	}

	// first pass: get allocation size info
	u32 numPositions = 0,
		numNormals = 0;

	char line[256];
	while (!feof(fpObj)) {
		fgets(line, 4, fpObj);

		if (strncmp(line, "v ", 2) == 0)
			numPositions++;
		if (strncmp(line, "vn ", 3) == 0)
			numNormals++;

		if (strncmp(line, "f ", 2) == 0)
			mesh.numFaces++;
	}

	printf("Positions: %u\nNormals: %u\nFaces: %u\n", numPositions, numNormals, mesh.numFaces);

	rewind(fpObj);

	// second pass: get the data

	// todo: dumb hack. the while loop below writes over the nominal size of the face
	// buffer we need, so just give it a lil extra padding. fix later?
	mesh.faces = malloc(sizeof(face_t) * mesh.numFaces + sizeof(face_t));
	vec3* positions = malloc(sizeof(vec3) * numPositions);
	vec3* normals = malloc(sizeof(vec3) * numNormals);
	u32 fi = 0,
		pi = 0,
		ni = 0;

	vec3 in_position,
		in_normal;
	u32 position_indices[3],
		normal_indices[3];


	while (!feof(fpObj)) {
		fgets(line, 256, fpObj);

		// upper section: gather data
		if (sscanf(line, "mtllib %s\n", line) == 1) {
			fpMat = fopen(line, "rb");
		}

		if (sscanf(line, "v %f %f %f\n",
			&in_position[0], &in_position[1], &in_position[2]) == 3) {
			memcpy(positions[pi++], in_position, sizeof(vec3));
		}

		if (sscanf(line, "vn %f %f %f\n",
			&in_normal[0], &in_normal[1], &in_normal[2]) == 3) {
			memcpy(normals[ni++], in_normal, sizeof(vec3));
		}

		// lower section: build faces
		if (sscanf(line, "f %i/%*i/%i %i/%*i/%i %i/%*i/%i\n",
			&position_indices[0], &normal_indices[0], 
			&position_indices[1], &normal_indices[1], 
			&position_indices[2], &normal_indices[2]) == 6) {
			vec3_dup(mesh.faces[fi][0].pos, positions[position_indices[0] - 1]);
			vec3_dup(mesh.faces[fi][1].pos, positions[position_indices[1] - 1]);
			vec3_dup(mesh.faces[fi][2].pos, positions[position_indices[2] - 1]);

			vec3_dup(mesh.faces[fi][0].norm, normals[normal_indices[0] - 1]);
			vec3_dup(mesh.faces[fi][1].norm, normals[normal_indices[1] - 1]);
			vec3_dup(mesh.faces[fi][2].norm, normals[normal_indices[2] - 1]);

			fi++;
		}
	}

	fclose(fpObj);

	// read material data from file if available
	if (fpMat) {
		vec3 params;
		char fpsDiffuseTex[256];

		while (!feof(fpMat)) {
			fgets(line, 256, fpMat);

			if (sscanf(line, "Kd %f %f %f\n",
				&params[0], &params[1], &params[2]) == 3) {
				vec3_dup(mesh.material.diffuse, params);
			}

			if (sscanf(line, "map_Kd %s\n", fpsDiffuseTex) == 1) {
				mesh.material.diffuse_tex = gfx_load_texture(fpsDiffuseTex);
			}
		}

		fclose(fpMat);
	}

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(face_t) * mesh.numFaces, mesh.faces,
		GL_STATIC_DRAW);

	free(positions);
	free(normals);

	return mesh;
}

void gfx_free_mesh(mesh_t* mesh) {
	glDeleteBuffers(1, &mesh->vbo);
	free(mesh->faces);
	mesh->numFaces = 0;
}

u32 gfx_load_texture(const char* filepath) {
	u32 tex = 0;
	FILE* fpTex = fopen(filepath, "rb");

	if (!fpTex)
		goto load_fail;

	int w, h;
	u8* data = stbi_load(filepath, &w, &h, 0, 4);
	if (!data)
		goto load_fail;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);

	return tex;

load_fail:
	printf("Failed to load texture: %s\n", filepath);
	return 0;
}

void gfx_free_texture(u32 tex) {

}
