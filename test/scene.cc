/**
 * @file scene.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "scene.h"

#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

static bool ready = 0;
static int width = 0;
static int height = 0;
static unsigned int frame_count = 0;

// resources
static GLuint program = 0;
static GLuint vertex_shader = 0;
static GLuint fragment_shader = 0;
static GLuint position_attribute_index = 0;
static GLuint color_attribute_index = 1;
static GLuint mvp_location = 0;
static GLuint vao = 0;

struct vertex_t {
	GLfloat position[4];
	GLfloat color[4];
};

static struct vertex_t vertex_data[] = {
	{ { -0.6f, -0.4f, 0.0f, 1.0 }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.6f, -0.4f, 0.0f, 1.0 }, { 0.0f, 1.0f, 0.0f } },
	{ {  0.0f,  0.6f, 0.0f, 1.0 }, { 0.0f, 0.0f, 1.0f } },
};

int scene_init(void)
{
	if (ready)
		return 0;

	const GLubyte* version;
	version = glGetString(GL_VERSION);
	if (!version) {
		fprintf(stderr, "GL version not available\n");
		return -1;
	} else {
		printf("%s\n", version);
	}

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "glewInit(): '%s'\n", glewGetErrorString(res));
		return -1;
	}

	ready = true;

	// depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return 0;
}

void scene_resize(int _width, int _height)
{
	printf("%s(); width=%d; height=%d\n", __FUNCTION__, _width, _height);

	width = _width;
	height = _height;
}

static const char* scene_read_shader(const std::string& filename)
{
	int r;
	struct stat st;
	FILE* f;

	r = stat(filename.c_str(), &st);
	if (r < 0) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
		return NULL;
	}

	f = fopen(filename.c_str(), "r");
	if (!f) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
		return NULL;
	}

	char* buf = new char[st.st_size + 1];

	r = fread(buf, st.st_size, 1, f);
	if (r != 1) {
		fprintf(stderr, "Failed to read %s\n", filename.c_str());
		return NULL;
	}
	buf[st.st_size] = 0;

	fclose(f);

	return buf;
}

static GLuint scene_load_shader(const std::string filename, GLenum shader_type)
{
	const char* source;
	GLuint shader;
	GLint source_len;
	GLint info_log_len = 0;
	GLint compile_status;

	source = scene_read_shader(filename);
	if (!source)
		goto error;

	shader = glCreateShader(shader_type);
	if (!shader) {
		fprintf(stderr, "glCreateShader() failed\n");
		goto error;
	}

	source_len = std::strlen(source);
	glShaderSource(shader, 1, &source, &source_len);
	glCompileShader(shader);

	// retrieve shader log
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
	GLchar info_log[info_log_len];
	glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);

	// check shader compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		fprintf(stderr, "Failed to compile shader:\n");

		GLint shader_source_len;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &shader_source_len);
		GLchar shader_source[shader_source_len];
		glGetShaderSource(shader, sizeof(shader_source), NULL, shader_source);
		fprintf(stderr, "%s\n", shader_source);

		fprintf(stderr, "%s: %s\n", filename.c_str(), info_log);

		glDeleteShader(shader);
		goto error;
	} else if (info_log_len > 1) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), info_log);
	}

	return shader;

error:
	delete[] source;
	return 0;
}

static GLuint scene_load_vao(const void* buf, size_t len)
{
	GLuint vao;
	GLuint vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, len, buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(position_attribute_index);
	glVertexAttribPointer(position_attribute_index, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), 0);
	glEnableVertexAttribArray(color_attribute_index);
	glVertexAttribPointer(color_attribute_index, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), (const GLvoid*)offsetof(struct vertex_t, color));

	glBindVertexArray(0);

	printf("%s(); buf=%p; len=%zu; vao=%u; vbo=%u\n", __FUNCTION__, buf, len, vao, vbo);

	return vao;
}

int scene_load_resources(void)
{
	program = glCreateProgram();
	if (!program) {
		fprintf(stderr, "glCreateProgram() failed\n");
		return -1;
	}

	vertex_shader = scene_load_shader("test/simple_vs.glsl", GL_VERTEX_SHADER);
	if (!vertex_shader) {
		fprintf(stderr, "Failed to load vertex shader\n");
		return -1;
	}

	fragment_shader = scene_load_shader("test/simple_fs.glsl", GL_FRAGMENT_SHADER);
	if (!fragment_shader) {
		fprintf(stderr, "Failed to load fragment shader\n");
		return -1;
	}

	// add shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// assign attributes
	glBindAttribLocation(program, position_attribute_index, "position");
	glBindAttribLocation(program, color_attribute_index, "color");

	// link program
	GLint link_status;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		GLint info_log_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
		GLchar info_log[info_log_len];
		glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Error linking program: %s\n", info_log);
		return -1;
	}

	// validate program
	GLint validate_status;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status != GL_TRUE) {
		GLint info_log_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
		GLchar info_log[info_log_len];
		glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Invalid program: %s\n", info_log);
		return -1;
	}

	glUseProgram(program);
	mvp_location = glGetUniformLocation(program, "mvp");

	vao = scene_load_vao(vertex_data, sizeof(vertex_data));

	return 0;
}

void scene_unload_resources(void)
{
	if (vao)
		glDeleteVertexArrays(1, &vao);
	if (vertex_shader)
		glDeleteShader(vertex_shader);
	if (fragment_shader)
		glDeleteShader(fragment_shader);
	if (program)
		glDeleteProgram(program);
}

void scene_render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 model = glm::rotate(glm::mat4(), (float)frame_count, glm::vec3(0.f, 0.f, 1.0f));
	glm::mat4 mvp = projection * view * model;

	glUseProgram(program);
	glBindVertexArray(vao);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

	++frame_count;
}
