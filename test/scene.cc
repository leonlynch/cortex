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

#include <string>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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
static GLuint normal_attribute_index = 1;
static GLuint vao = 0;

// uniform location map
typedef std::map<std::string, GLint> uniform_location_t;
static uniform_location_t uniform_location;
static GLint uniform_count = 0;
static GLint uniform_max_length = 0;

struct vertex_t {
	GLfloat position[3];
	GLfloat normal[3];
};

static struct vertex_t vertex_data[] = {
	{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
	{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
	{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
	{ {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },

	{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
	{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
	{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },

	{ {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f } },

	{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f } },
	{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f } },

	{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },

	{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f } },
	{ { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f } },
};

static GLuint index_data[] = {
	0, 1, 3,
	2, 3, 1,

	4, 5, 7,
	6, 7, 5,

	8, 9, 11,
	10, 11, 9,

	12, 13, 15,
	14, 15, 13,

	16, 17, 19,
	18, 19, 17,

	20, 21, 23,
	22, 23, 21,
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

	// back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return 0;
}

void scene_resize(int _width, int _height)
{
	printf("%s(); width=%d; height=%d\n", __FUNCTION__, _width, _height);

	width = _width;
	height = _height;
}

static int scene_read_shader(const std::string& filename, std::string& source)
{
	int r;
	struct stat st;
	FILE* f;

	r = stat(filename.c_str(), &st);
	if (r < 0) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
		return -1;
	}

	f = fopen(filename.c_str(), "r");
	if (!f) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
		return -1;
	}

	char buf[st.st_size + 1];

	r = fread(buf, st.st_size, 1, f);
	if (r != 1) {
		fprintf(stderr, "Failed to read %s\n", filename.c_str());
		goto error;
	}
	buf[st.st_size] = 0;
	source = buf;

	r = 0;
	goto exit;

error:
	r = -1;
exit:
	fclose(f);
	return r;
}

static GLuint scene_load_shader(const std::string& filename, GLenum shader_type)
{
	int r;
	std::string source;
	GLuint shader;
	const char* source_ptr;
	GLint info_log_len = 0;
	GLint compile_status;

	r = scene_read_shader(filename, source);
	if (r)
		goto error;

	shader = glCreateShader(shader_type);
	if (!shader) {
		fprintf(stderr, "glCreateShader() failed\n");
		goto error;
	}

	// compile shader
	source_ptr = source.c_str();
	glShaderSource(shader, 1, &source_ptr, NULL);
	glCompileShader(shader);

	// retrieve shader info log
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
		goto error2;
	} else if (info_log_len > 1) {
		fprintf(stderr, "%s: %s\n", filename.c_str(), info_log);
	}

	goto exit;

error2:
	glDeleteShader(shader);
error:
	shader = 0;
exit:
	return shader;
}

static GLuint scene_load_vao(const void* vertex_data, size_t vertex_data_len, const void* index_data, size_t index_data_len)
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_len, vertex_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(position_attribute_index);
	glVertexAttribPointer(position_attribute_index, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), 0);
	glEnableVertexAttribArray(normal_attribute_index);
	glVertexAttribPointer(normal_attribute_index, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), (const GLvoid*)offsetof(struct vertex_t, normal));

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_len, index_data, GL_STATIC_DRAW);

	glBindVertexArray(0);

	printf("%s(); vao=%u; vbo=%u; ibo=%u\n", __FUNCTION__, vao, vbo, ibo);

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
	glBindAttribLocation(program, normal_attribute_index, "normal");

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

	// lookup all uniforms
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_max_length);
	for (int i = 0; i < uniform_count; ++i) {
		GLchar uniform_name[uniform_max_length];
		GLint uniform_size = 0;
		GLenum uniform_type = 0;
		glGetActiveUniform(program, i, sizeof(uniform_name), NULL, &uniform_size, &uniform_type, uniform_name);
		uniform_location[uniform_name] = glGetUniformLocation(program, uniform_name);
		printf("%s(); uniform='%s'; size=%d; type=%d; location=%d\n", __FUNCTION__, uniform_name, uniform_size, uniform_type, uniform_location[uniform_name]);
	}

	vao = scene_load_vao(vertex_data, sizeof(vertex_data), index_data, sizeof(index_data));

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
	glUseProgram(program);

	// uniform matrices
	glm::mat4 m_projection = glm::perspective(45.0f, width / (float)height, 0.1f, 100.0f);
	glm::mat4 m_view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 m_model_rotate_x = glm::rotate(glm::mat4(), (float)frame_count, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 m_model_rotate_y = glm::rotate(glm::mat4(), (float)frame_count, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_model_rotate_z = glm::rotate(glm::mat4(), (float)frame_count / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m_model = m_model_rotate_z * m_model_rotate_y * m_model_rotate_x;
	glm::mat4 m_modelview = m_view * m_model;
	glm::mat4 m_mvp = m_projection * m_modelview;
	glm::mat3 m_normal = glm::inverseTranspose(glm::mat3(m_modelview));

	glUniformMatrix4fv(uniform_location["m_mvp"], 1, GL_FALSE, glm::value_ptr(m_mvp));
	glUniformMatrix4fv(uniform_location["m_modelview"], 1, GL_FALSE, glm::value_ptr(m_modelview));
	glUniformMatrix3fv(uniform_location["m_normal"], 1, GL_FALSE, glm::value_ptr(m_normal));

	// uniform light parameters
	glm::vec4 light_position = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
	glm::vec3 light_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 light_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

	glUniform4fv(uniform_location["light.position"], 1, glm::value_ptr(light_position));
	glUniform3fv(uniform_location["light.ambient"], 1, glm::value_ptr(light_ambient));
	glUniform3fv(uniform_location["light.diffuse"], 1, glm::value_ptr(light_diffuse));
	glUniform3fv(uniform_location["light.specular"], 1, glm::value_ptr(light_specular));

	// uniform material parameters
	glm::vec3 material_ambient = glm::vec3(0.2f, 0.0f, 0.0f);
	glm::vec3 material_diffuse = glm::vec3(0.8f, 0.0f, 0.0f);
	glm::vec3 material_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float material_shininess = 25;

	glUniform3fv(uniform_location["material.ambient"], 1, glm::value_ptr(material_ambient));
	glUniform3fv(uniform_location["material.diffuse"], 1, glm::value_ptr(material_diffuse));
	glUniform3fv(uniform_location["material.specular"], 1, glm::value_ptr(material_specular));
	glUniform1f(uniform_location["material.shininess"], material_shininess);

	// render
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, sizeof(index_data) / sizeof(index_data[0]), GL_UNSIGNED_INT, 0);

	// cleanup
	glBindVertexArray(0);
	glUseProgram(0);

	++frame_count;
}
