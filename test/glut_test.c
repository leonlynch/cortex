/**
 * @file glut_test.c
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <GL/glut.h>

#include "scene.h"

static void reshape_func(int width, int height)
{
	scene_resize(width, height);
}

static void timer_func(int value)
{
	scene_update();

	glutTimerFunc(20, &timer_func, 20);
}

static void display_func(void)
{
	scene_render();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	int r;

	glutInit(&argc, argv);
// 	glutInitContextVersion(3, 2);
// 	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("GLUT test");

	glutReshapeFunc(&reshape_func);
	glutTimerFunc(20, &timer_func, 20);
	glutDisplayFunc(&display_func);
	glutIdleFunc(&display_func);

	r = scene_init();
	if (r)
		return 1;
	r = scene_load_resources();
	if (r)
		return 1;

	glutMainLoop();

	scene_unload_resources();

	return 0;
}
