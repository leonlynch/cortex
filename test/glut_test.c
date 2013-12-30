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

static void display_func(void)
{
	scene_render();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
// 	glutInitContextVersion(3, 2);
// 	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("GLUT test");

	glutReshapeFunc(&reshape_func);
	glutDisplayFunc(&display_func);
	glutIdleFunc(&display_func);

	scene_init();
	scene_load_resources();

	glutMainLoop();

	scene_unload_resources();

	return 0;
}
