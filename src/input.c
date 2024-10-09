#include "input.h"
#include "display.h"

#include <GLFW/glfw3.h>

b8 input_get_key(input_key key) {
	return glfwGetKey((GLFWwindow*)display_get_window(), key);
}