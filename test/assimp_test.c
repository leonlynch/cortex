/**
 * @file assimp_test.c
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <stdbool.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#include "assimp_scene.h"

static bool scene_is_paused = false;
static bool render_wireframe = false;
static bool is_fullscreen = false;
static int windowed_x;
static int windowed_y;
static int windowed_width;
static int windowed_height;

static bool mouse_dragging = false;
static double mouse_last_x;
static double mouse_last_y;

static void glfw_error_func(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

static void glfw_resize_func(GLFWwindow* window, int width, int height)
{
	scene_resize(width, height);
}

static void glfw_key_func(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		scene_is_paused ^= true;
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS) { // not 'w', to allow WASD later
		render_wireframe ^= true;
		scene_set_wireframe(render_wireframe);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (is_fullscreen) {
			glfwSetWindowMonitor(window, NULL, windowed_x, windowed_y, windowed_width, windowed_height, 0);
			is_fullscreen = false;
		} else {
			glfwGetWindowPos(window, &windowed_x, &windowed_y);
			glfwGetWindowSize(window, &windowed_width, &windowed_height);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			is_fullscreen = true;
		}
	}
}

static void glfw_mouse_button_func(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouse_dragging = true;
			glfwGetCursorPos(window, &mouse_last_x, &mouse_last_y);
		} else if (action == GLFW_RELEASE) {
			mouse_dragging = false;
		}
	}
}

static void glfw_cursor_pos_func(GLFWwindow* window, double x, double y)
{
	if (mouse_dragging) {
		scene_rotate(
			(float)(x - mouse_last_x) * 0.3f,
			(float)(y - mouse_last_y) * 0.3f
		);
	}
	mouse_last_x = x;
	mouse_last_y = y;
}

static void glfw_scroll_func(GLFWwindow* window, double xoffset, double yoffset)
{
	scene_zoom((float)(-yoffset) * 0.5f);
}


int main(int argc, char** argv)
{
	printf("GLFW: %s\n", glfwGetVersionString());

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <model_file>\n", argv[0]);
		return 1;
	}

	glfwSetErrorCallback(glfw_error_func);

	int r;
	GLFWwindow* window;

	r = glfwInit();
	if (!r) {
		fprintf(stderr, "glfwInit() failed; r=%d\n", r);
		r = 1;
		goto exit;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(640, 480, "assimp test", NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwCreateWindow() failed\n");
		r = 1;
		goto exit2;
	}
	glfwSetFramebufferSizeCallback(window, glfw_resize_func);
	glfwSetKeyCallback(window, glfw_key_func);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_func);
	glfwSetCursorPosCallback(window, glfw_cursor_pos_func);
	glfwSetScrollCallback(window, glfw_scroll_func);

	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	r = scene_init();
	if (r) {
		goto exit3;
	}
	r = scene_load_resources(argv[1]);
	if (r) {
		goto exit3;
	}
	scene_resize(width, height);

	while (!glfwWindowShouldClose(window))
	{
		if (!scene_is_paused) {
			scene_update();
		}

		scene_render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	scene_unload_resources();

	r = 0;

exit3:
	glfwDestroyWindow(window);
exit2:
	glfwTerminate();
exit:
	return r;
}
