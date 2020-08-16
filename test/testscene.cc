/**
 * @file testscene.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "testscene.h"

#include <cstdio>
#include <cstring>

#include <string>
#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

static bool ready = 0;
static int width = 0;
static int height = 0;
static unsigned int tick = 0;

// resources
static GLuint program = 0;
static GLuint vertex_shader = 0;
static GLuint fragment_shader = 0;

// uniform location map
typedef std::map<std::string, GLint> uniform_location_t;
static uniform_location_t uniform_location;
static GLint uniform_count = 0;
static GLint uniform_max_length = 0;

// attribute location map
typedef std::map<std::string, GLint> attribute_location_t;
static attribute_location_t attribute_location;
static GLint attribute_count = 0;
static GLint attribute_max_length = 0;

// fragment data output
static GLint fragdata_location = 0;

struct vertex_t {
	glm::vec3 position;
	glm::vec3 normal;
};

struct mesh_t {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	GLsizei index_count = 0;
};

// cube vertices, grouped by face
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

// cube vertex indices, grouped by face
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

// cube mesh
static mesh_t cube_mesh;

static void scene_debug(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
)
{
	fprintf(stderr, "source=%u; type=%u; id=%u; severity=%u; length=%d; message='%s'\n", source, type, id, severity, length, message);
}

int scene_init(void)
{
	if (ready)
		return 0;

	const GLubyte* version;
	version = glGetString(GL_VERSION);
	if (!version) {
		fprintf(stderr, "GL version not available\n");
		return -1;
	}
	printf("OpenGL: %s\n", version);

	const GLubyte* renderer;
	renderer = glGetString(GL_RENDERER);
	if (!renderer) {
		fprintf(stderr, "GL renderer not available\n");
		return -1;
	}
	printf("Renderer: %s\n", renderer);

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "glewInit(): '%s'\n", glewGetErrorString(res));
		return -1;
	}

	// debugging
	if (GLEW_KHR_debug) { // KHR_debug is part of OpenGL-4.3 core
		glDebugMessageCallback(&scene_debug, NULL);
		glEnable(GL_DEBUG_OUTPUT);
		std::printf("Debug enabled\n");
	}

	// depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	ready = true;

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
	std::vector<GLchar> info_log;
	GLint compile_status = GL_FALSE;

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
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetShaderInfoLog(shader, info_log.size(), NULL, info_log.data());
	}

	// check shader compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		fprintf(stderr, "Failed to compile shader:\n");

		GLint shader_source_len;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &shader_source_len);
		GLchar shader_source[shader_source_len];
		glGetShaderSource(shader, sizeof(shader_source), NULL, shader_source);
		fprintf(stderr, "%s\n", shader_source);

		if (!info_log.empty()) {
			fprintf(stderr, "%s: %s\n", filename.c_str(), info_log.data());
		}

		goto error2;
	} else if (!info_log.empty()) {
		printf("%s: %s\n", filename.c_str(), info_log.data());
	}

	goto exit;

error2:
	glDeleteShader(shader);

error:
	shader = 0;

exit:
	return shader;
}

static void scene_load_mesh(const void* vertex_data, size_t vertex_data_len, const void* index_data, size_t index_data_len, mesh_t* mesh)
{
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_len, vertex_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attribute_location["v_position"]);
	glVertexAttribPointer(attribute_location["v_position"], 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), 0);
	glEnableVertexAttribArray(attribute_location["v_normal"]);
	glVertexAttribPointer(attribute_location["v_normal"], 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), (const GLvoid*)offsetof(struct vertex_t, normal));

	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_len, index_data, GL_STATIC_DRAW);
	mesh->index_count = index_data_len / sizeof(GLuint);

	glBindVertexArray(0);

	printf("%s(); vao=%u; vbo=%u; ibo=%u\n", __FUNCTION__, mesh->vao, mesh->vbo, mesh->ibo);
}

int scene_load_resources(void)
{
	GLint link_status = GL_FALSE;
	GLint validate_status = GL_FALSE;
	GLint info_log_len;
	std::vector<GLchar> info_log;

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

	// link program
	glLinkProgram(program);
	info_log_len = 0;
	info_log.clear();
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetProgramInfoLog(program, info_log.size(), NULL, info_log.data());
	}

	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		fprintf(stderr, "Error linking shader program:\n%s\n", info_log.data());
		return -1;
	}
	if (!info_log.empty()) {
		printf("Shader program log:\n%s\n", info_log.data());
	}

	// validate program
	glValidateProgram(program);
	info_log_len = 0;
	info_log.clear();
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetProgramInfoLog(program, info_log.size(), NULL, info_log.data());
	}

	glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status != GL_TRUE) {
		fprintf(stderr, "Invalid program:\n%s\n", info_log.data());
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

	// lookup all attributes
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribute_count);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribute_max_length);
	for (int i = 0; i < attribute_count; ++i) {
		GLchar attribute_name[attribute_max_length];
		GLint attribute_size = 0;
		GLenum attribute_type = 0;
		glGetActiveAttrib(program, i, sizeof(attribute_name), NULL, &attribute_size, & attribute_type, attribute_name);
		attribute_location[attribute_name] = glGetAttribLocation(program, attribute_name);
		printf("%s(); attribute='%s'; size=%d; type=%d; location=%d\n", __FUNCTION__, attribute_name, attribute_size, attribute_type, attribute_location[attribute_name]);
	}

	// lookup fragment output
	fragdata_location = glGetFragDataLocation(program, "color");
	printf("%s(); fragdata='%s'; location=%d\n", __FUNCTION__, "color", fragdata_location);

	// load cube mesh
	scene_load_mesh(vertex_data, sizeof(vertex_data), index_data, sizeof(index_data), &cube_mesh);

	return 0;
}

static void scene_unload_mesh(mesh_t* mesh)
{
	if (mesh->vao) {
		glDeleteVertexArrays(1, &mesh->vao);
		mesh->vao = 0;
	}

	if (mesh->vbo) {
		glDeleteBuffers(1, &mesh->vbo);
		mesh->vbo = 0;
	}

	if (mesh->ibo) {
		glDeleteBuffers(1, &mesh->ibo);
		mesh->ibo = 0;
	}
}

void scene_unload_resources(void)
{
	scene_unload_mesh(&cube_mesh);

	if (vertex_shader) {
		glDeleteShader(vertex_shader);
	}
	if (fragment_shader) {
		glDeleteShader(fragment_shader);
	}
	if (program) {
		glDeleteProgram(program);
	}
}

void scene_update(void)
{
	++tick;
}

void scene_render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);
	glUseProgram(program);

	// uniform matrices
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 m_model_rotate_x = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 m_model_rotate_y = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_model_rotate_z = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
	glBindVertexArray(cube_mesh.vao);
	glDrawElements(GL_TRIANGLES, cube_mesh.index_count, GL_UNSIGNED_INT, 0);

	// cleanup
	glBindVertexArray(0);
	glUseProgram(0);
}
