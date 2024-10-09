#include "display.h"
#include "common.h"
#include "engine.h"

#include <GLFW/glfw3.h>

void glfwWindowCloseCallback(GLFWwindow* window);

typedef struct {
	GLFWwindow* window;
} display_ctx_t;

static display_ctx_t* g_display = 0;

void display_init() {
	g_display = calloc(1, sizeof(display_ctx_t));

	glfwInit();

	g_display->window = glfwCreateWindow(800, 600, "silvus", 0, 0);
	glfwMakeContextCurrent(g_display->window);
	glfwSwapInterval(1);

	glfwSetWindowCloseCallback(g_display->window, glfwWindowCloseCallback);
}

void display_fin() {
	glfwDestroyWindow(g_display->window);
	glfwTerminate();

	free(g_display);
}

void display_poll_events() {
	glfwPollEvents();
}

void display_present() {
	glfwSwapBuffers(g_display->window);
}

void* display_get_window() {
	return g_display->window;
}


void glfwWindowCloseCallback(GLFWwindow* window) {
	engine_quit();
}