#include "shaders/shader_common.h"

glsl(glsl_default_vs,

	layout (location = 0) in vec3 a_pos;

	layout (std140, binding = 0) uniform mvp {
		mat4 viewProjection;
		mat4 model;
	} u_mvp;

	void main() {
		gl_Position = u_mvp.viewProjection * u_mvp.model * vec4(a_pos, 1.0);
	}

)

glsl(glsl_default_fs,
	
	layout (location = 0) out vec4 o_fragColor;

	void main() {
		o_fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}

)