/**
 * @file glfw_test.c
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#include "testscene.h"

static bool scene_is_paused = false;
static enum scene_demo_t current_scene_demo = SCENE_DEMO_CUBE;
static bool render_normals = false;
static bool render_wireframe = false;
static int subdivision_delta = 0;
static bool is_fullscreen = false;
static int windowed_x;
static int windowed_y;
static int windowed_width;
static int windowed_height;

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

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		current_scene_demo = scene_next_demo(current_scene_demo);
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		render_normals ^= true;
		scene_set_normals(render_normals);
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS) { // not 'w', to allow WASD later
		render_wireframe ^= true;
		scene_set_wireframe(render_wireframe);
	}

	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
		scene_set_complexity(--subdivision_delta);
	}

	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
		scene_set_complexity(++subdivision_delta);
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


int main(int argc, char** argv)
{
	printf("GLFW: %s\n", glfwGetVersionString());

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

	window = glfwCreateWindow(640, 480, "GLFW test", NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwCreateWindow() failed\n");
		r = 1;
		goto exit2;
	}
	glfwSetFramebufferSizeCallback(window, glfw_resize_func);
	glfwSetKeyCallback(window, glfw_key_func);

	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	r = scene_init();
	if (r)
		goto exit3;
	r = scene_load_resources();
	if (r)
		goto exit3;
	scene_resize(width, height);

	while (!glfwWindowShouldClose(window))
	{
		if (!scene_is_paused) {
			scene_update();
		}

		scene_render(current_scene_demo);

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
