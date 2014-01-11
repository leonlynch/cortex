/**
 * @file glfw_test.c
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#include "scene.h"

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
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


int main(int argc, char** argv)
{
	printf("%s\n", glfwGetVersionString());

	glfwSetErrorCallback(glfw_error_func);

	int r;
	GLFWwindow* window;

	r = glfwInit();
	if (!r) {
		fprintf(stderr, "glfwInit() failed; r=%d\n", r);
		r = 1;
		goto exit;
	}

// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 32);

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
