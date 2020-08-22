/**
 * @file glut_test.c
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "testscene.h"

static enum scene_demo_t current_scene_demo = SCENE_DEMO_CUBE;
static bool render_normals = false;
static bool render_wireframe = false;

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
	scene_render(current_scene_demo);
	glutSwapBuffers();
}

static void keyboard_func(unsigned char key, int x, int y)
{
	switch (key) {
		case 0x1b:
			glutLeaveMainLoop();
			break;

		case 'm':
			current_scene_demo = scene_next_demo(current_scene_demo);
			break;

		case 'n':
			render_normals ^= true;
			scene_set_normals(render_normals);
			break;

		case 'b': // not 'w', to allow WASD later
			render_wireframe ^= true;
			scene_set_wireframe(render_wireframe);
			break;
	}
}

int main(int argc, char** argv)
{
	int r;

	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("GLUT test");

	glutReshapeFunc(&reshape_func);
	glutTimerFunc(20, &timer_func, 20);
	glutDisplayFunc(&display_func);
	glutIdleFunc(&display_func);
	glutKeyboardFunc(&keyboard_func);

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
