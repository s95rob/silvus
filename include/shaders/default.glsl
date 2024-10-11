#include "shaders/shader_common.h"

glsl(glsl_default_vs,

	layout (location = 0) in vec3 a_pos;
	layout (location = 1) in vec3 a_norm;

	layout (std140, binding = 0) uniform mvp {
		mat4 viewProjection;
		mat4 model;
	} u_mvp;

	layout (location = 0) out vec3 v_color;

	void main() {
		gl_Position = u_mvp.viewProjection * u_mvp.model * vec4(a_pos, 1.0);
		v_color = a_norm;
	}

)

glsl(glsl_default_fs,
	
	layout (location = 0) out vec4 o_fragColor;

	layout (std140, binding = 1) uniform mat {
		vec3 diffuse;
	} u_mat;

	layout (location = 0) in vec3 v_color;

	void main() {
		o_fragColor = vec4(vec3(0.8), 1.0);
	}

)