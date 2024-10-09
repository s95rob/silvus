#include "shaders/shader_common.h"

glsl(glsl_sprite_vs, 
    
    layout (location = 0) in vec2 a_pos;

    layout (std140, binding = 0) uniform mvp {
        mat4 viewProjection;
    } u_mvp;

    void main() {
	    gl_Position = u_mvp.viewProjection * vec4(a_pos, 0.0, 1.0);
    }
)

glsl(glsl_sprite_fs,

    layout(location = 0) out vec4 o_fragColor;

    void main() {
	    o_fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
)