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
#include <type_traits>

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

#include "bezier.h"
#include "teaset.h"
#include "sphere.h"
#include "shape.h"

static bool ready = 0;
static int width = 0;
static int height = 0;
static unsigned int tick = 0;
static bool render_normals = false;

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

struct normals_t {
	GLuint vao = 0;

	GLuint vbo = 0;
	GLsizei vertex_count = 0;
};

struct mesh_t {
	GLuint vao = 0;

	GLuint vbo = 0;
	GLsizei vertex_count = 0;

	GLuint ibo = 0;
	GLsizei index_count = 0;

	normals_t normals;
};

// cube mesh
static Cube<glm::vec3> cube;
static std::vector<vertex_t> cube_vertices;
static std::vector<unsigned int> cube_indices;
static mesh_t cube_mesh;

// octahedron mesh
static Octahedron<glm::vec3> octahedron;
static std::vector<vertex_t> octahedron_vertices;
static std::vector<unsigned int> octahedron_indices;
static mesh_t octahedron_mesh;

// bezier surface mesh
static glm::vec3 bezier_surface_data[4][4] = {
	{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 2.0f, 0.0f }, { 0.0f, 3.0f, 0.0f }, },
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 2.0f, 1.0f }, { 1.0f, 3.0f, 0.0f }, },
	{ { 2.0f, 0.0f, 0.0f }, { 2.0f, 1.0f, 1.0f }, { 2.0f, 2.0f, 1.0f }, { 2.0f, 3.0f, 0.0f }, },
	{ { 3.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 0.0f }, { 3.0f, 2.0f, 0.0f }, { 3.0f, 3.0f, 0.0f }, },
};

static BezierSurface<glm::vec3, 3, 3> bezier_surface(bezier_surface_data);
static std::vector<vertex_t> bezier_surface_vertices;
static std::vector<unsigned int> bezier_surface_indices;
static mesh_t bezier_surface_mesh;

// Utah teapot
static Teapot teapot;
static std::vector<vertex_t> teapot_vertices;
static std::vector<unsigned int> teapot_indices;
static mesh_t teapot_mesh;

// Utah teacup
static Teacup teacup;
static std::vector<vertex_t> teacup_vertices;
static std::vector<unsigned int> teacup_indices;
static mesh_t teacup_mesh;

// Utah teaspoon
static Teaspoon teaspoon;
static std::vector<vertex_t> teaspoon_vertices;
static std::vector<unsigned int> teaspoon_indices;
static mesh_t teaspoon_mesh;

// Sphere
static Sphere<glm::vec3> sphere;
static std::vector<vertex_t> sphere_vertices;
static std::vector<unsigned int> sphere_indices;
static mesh_t sphere_mesh;

// helper function declarations
static void scene_update_mesh(const std::vector<vertex_t>& vertices, const std::vector<unsigned int>& indices, mesh_t* mesh);
static void scene_update_mesh_normals(const std::vector<vertex_t>& vertices, normals_t* normals);

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

	// multisampling
	glEnable(GL_MULTISAMPLE);
	GLint msaa = 0;
	glGetIntegerv(GL_SAMPLES, &msaa);
	if (msaa > 0) {
		std::printf("MSAAx%d enabled\n", msaa);
	} else {
		std::printf("MSAA disabled\n");
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

static void scene_load_mesh(const std::vector<vertex_t>& vertices, const std::vector<unsigned int>& indices, mesh_t* mesh)
{
	// generate new vertex array object
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// generate and setup new vertex buffer object
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glEnableVertexAttribArray(attribute_location["v_position"]);
	glVertexAttribPointer(attribute_location["v_position"], 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), 0);
	glEnableVertexAttribArray(attribute_location["v_normal"]);
	glVertexAttribPointer(attribute_location["v_normal"], 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), (const GLvoid*)offsetof(struct vertex_t, normal));

	// generate new index buffer object
	glGenBuffers(1, &mesh->ibo);

	// load data
	scene_update_mesh(vertices, indices, mesh);

	// unbind vertex array object
	glBindVertexArray(0);
}

static void scene_update_mesh(const std::vector<vertex_t>& vertices, const std::vector<unsigned int>& indices, mesh_t* mesh)
{
	using vertex_type = std::remove_reference<decltype(vertices)>::type::value_type;
	using index_type = std::remove_reference<decltype(indices)>::type::value_type;

	// bind existing vertex array object
	glBindVertexArray(mesh->vao);

	// update existing vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_type), vertices.data(), GL_DYNAMIC_DRAW);
	mesh->vertex_count = vertices.size();

	// update existing index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(index_type), indices.data(), GL_DYNAMIC_DRAW);
	mesh->index_count = indices.size();

	// unbind vertex array object
	glBindVertexArray(0);

	printf("%s(); vao=%u; vbo=%u[%zu]; ibo=%u[%zu]\n", __FUNCTION__, mesh->vao, mesh->vbo, vertices.size(), mesh->ibo, indices.size());
}

static void scene_load_mesh_normals(const std::vector<vertex_t>& vertices, normals_t* normals)
{
	// generate new vertex array object
	glGenVertexArrays(1, &normals->vao);
	glBindVertexArray(normals->vao);

	// generate and setup new vertex buffer object
	glGenBuffers(1, &normals->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals->vbo);
	glEnableVertexAttribArray(attribute_location["v_position"]);
	glVertexAttribPointer(attribute_location["v_position"], 3, GL_FLOAT, GL_FALSE, 0, 0);

	// load data
	scene_update_mesh_normals(vertices, normals);

	// unbind vertex array object
	glBindVertexArray(0);
}

static void scene_update_mesh_normals(const std::vector<vertex_t>& vertices, normals_t* normals)
{
	// generate vertices representing normal lines
	using line_type = std::pair<glm::vec3,glm::vec3>;
	std::vector<line_type> normal_lines;
	normal_lines.reserve(vertices.size());
	for (auto&& vertex : vertices) {
		normal_lines.emplace_back(
			vertex.position,
			vertex.position + (glm::normalize(vertex.normal) * 0.3f)
		);
	}

	// bind existing vertex array object
	glBindVertexArray(normals->vao);

	// update existing vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, normals->vbo);
	glBufferData(GL_ARRAY_BUFFER, normal_lines.size() * sizeof(line_type), normal_lines.data(), GL_DYNAMIC_DRAW);
	normals->vertex_count = normal_lines.size() * 2; // two vertices per line

	printf("%s(); vao=%u; vbo=%u[%zu]\n", __FUNCTION__, normals->vao, normals->vbo, normal_lines.size());
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

	vertex_shader = scene_load_shader("test/simple.vert.glsl", GL_VERTEX_SHADER);
	if (!vertex_shader) {
		fprintf(stderr, "Failed to load vertex shader\n");
		return -1;
	}

	fragment_shader = scene_load_shader("test/simple.frag.glsl", GL_FRAGMENT_SHADER);
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
	cube.tesselate(cube_vertices, cube_indices);
	scene_load_mesh(cube_vertices, cube_indices, &cube_mesh);
	scene_load_mesh_normals(cube_vertices, &cube_mesh.normals);

	// load octahedron mesh
	octahedron.tesselate(octahedron_vertices, octahedron_indices);
	scene_load_mesh(octahedron_vertices, octahedron_indices, &octahedron_mesh);
	scene_load_mesh_normals(octahedron_vertices, &octahedron_mesh.normals);

	// load bezier surface mesh
	bezier_surface.tesselate(16, 16, bezier_surface_vertices, bezier_surface_indices);
	scene_load_mesh(bezier_surface_vertices, bezier_surface_indices, &bezier_surface_mesh);
	scene_load_mesh_normals(bezier_surface_vertices, &bezier_surface_mesh.normals);

	// load teapot mesh
	teapot.tesselate(12, 12, teapot_vertices, teapot_indices);
	scene_load_mesh(teapot_vertices, teapot_indices, &teapot_mesh);
	scene_load_mesh_normals(teapot_vertices, &teapot_mesh.normals);

	// load teacup mesh
	teacup.tesselate(8, 8, teacup_vertices, teacup_indices);
	scene_load_mesh(teacup_vertices, teacup_indices, &teacup_mesh);
	scene_load_mesh_normals(teacup_vertices, &teacup_mesh.normals);

	// load teaspoon mesh
	teaspoon.tesselate(8, 8, teaspoon_vertices, teaspoon_indices);
	scene_load_mesh(teaspoon_vertices, teaspoon_indices, &teaspoon_mesh);
	scene_load_mesh_normals(teaspoon_vertices, &teaspoon_mesh.normals);

	// load sphere mesh
	sphere.tesselate(3, sphere_vertices, sphere_indices);
	scene_load_mesh(sphere_vertices, sphere_indices, &sphere_mesh);
	scene_load_mesh_normals(sphere_vertices, &sphere_mesh.normals);

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

	if (mesh->normals.vao) {
		glDeleteVertexArrays(1, &mesh->normals.vao);
		mesh->normals.vao = 0;
	}

	if (mesh->normals.vbo) {
		glDeleteBuffers(1, &mesh->normals.vbo);
		mesh->normals.vbo = 0;
	}
}

void scene_unload_resources(void)
{
	scene_unload_mesh(&cube_mesh);
	scene_unload_mesh(&octahedron_mesh);
	scene_unload_mesh(&bezier_surface_mesh);
	scene_unload_mesh(&teapot_mesh);
	scene_unload_mesh(&teacup_mesh);
	scene_unload_mesh(&teaspoon_mesh);
	scene_unload_mesh(&sphere_mesh);

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

void scene_render(enum scene_demo_t scene_demo)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);
	glUseProgram(program);

	// uniform matrices
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));
	glm::mat4 m_model_rotate_x = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 m_model_rotate_y = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_model_rotate_z = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m_model = m_model_rotate_z * m_model_rotate_y * m_model_rotate_x;
	glm::mat4 m_modelview = m_view * m_model;
	glm::mat4 m_mvp = m_projection * m_modelview;
	glm::mat3 m_normal = glm::inverseTranspose(glm::mat3(m_modelview));

	glUniformMatrix4fv(uniform_location["m_mvp"], 1, GL_FALSE, glm::value_ptr(m_mvp));
	glUniformMatrix4fv(uniform_location["m_modelview"], 1, GL_FALSE, glm::value_ptr(m_modelview));
	glUniformMatrix3fv(uniform_location["m_normal"], 1, GL_FALSE, glm::value_ptr(m_normal));

	// uniform light parameters
	glm::vec4 light_position = glm::vec4(15.0f, 15.0f, 15.0f, 1.0f);
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

	// determine current mesh
	mesh_t* current_mesh = nullptr;
	switch (scene_demo) {
		case SCENE_DEMO_CUBE: current_mesh = &cube_mesh; break;
		case SCENE_DEMO_OCTAHEDRON: current_mesh = &octahedron_mesh; break;
		case SCENE_DEMO_BEZIER: current_mesh = &bezier_surface_mesh; break;
		case SCENE_DEMO_TEAPOT: current_mesh = &teapot_mesh; break;
		case SCENE_DEMO_TEACUP: current_mesh = &teacup_mesh; break;
		case SCENE_DEMO_TEASPOON: current_mesh = &teaspoon_mesh; break;
		case SCENE_DEMO_SPHERE: current_mesh = &sphere_mesh; break;
		default: current_mesh = &cube_mesh;
	}

	// render current mesh
	glBindVertexArray(current_mesh->vao);
	glDrawElements(GL_TRIANGLES, current_mesh->index_count, GL_UNSIGNED_INT, 0);

	if (render_normals && current_mesh->normals.vao) {
		// update uniform light parameters for normal lines
		light_ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		light_diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniform_location["light.ambient"], 1, glm::value_ptr(light_ambient));
		glUniform3fv(uniform_location["light.diffuse"], 1, glm::value_ptr(light_diffuse));

		// update uniform material parameters for normal lines
		material_ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		material_diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniform_location["material.ambient"], 1, glm::value_ptr(material_ambient));
		glUniform3fv(uniform_location["material.diffuse"], 1, glm::value_ptr(material_diffuse));

		// render current normals
		glBindVertexArray(current_mesh->normals.vao);
		glDrawArrays(GL_LINES, 0, current_mesh->normals.vertex_count);
	}

	// cleanup
	glBindVertexArray(0);
	glUseProgram(0);
}

enum scene_demo_t scene_next_demo(enum scene_demo_t current_demo)
{
	if (current_demo < SCENE_DEMO_SPHERE) {
		return static_cast<scene_demo_t>(static_cast<int>(current_demo) + 1);
	} else {
		return SCENE_DEMO_CUBE;
	}
}

void scene_set_normals(bool enabled)
{
	render_normals = enabled;
}

void scene_set_wireframe(bool enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}

void scene_set_complexity(int subdivision_delta)
{
	std::size_t sub_count;

	// clear data
	bezier_surface_vertices.clear();
	bezier_surface_indices.clear();
	teapot_vertices.clear();
	teapot_indices.clear();
	teacup_vertices.clear();
	teacup_indices.clear();
	teaspoon_vertices.clear();
	teaspoon_indices.clear();
	sphere_vertices.clear();
	sphere_indices.clear();

	// update bezier surface mesh
	sub_count = glm::clamp(16 + subdivision_delta, 2, 24);
	bezier_surface.tesselate(sub_count, sub_count, bezier_surface_vertices, bezier_surface_indices);
	scene_update_mesh(bezier_surface_vertices, bezier_surface_indices, &bezier_surface_mesh);
	scene_update_mesh_normals(bezier_surface_vertices, &bezier_surface_mesh.normals);

	// update teapot mesh
	sub_count = glm::clamp(12 + subdivision_delta, 2, 24);
	teapot.tesselate(sub_count, sub_count, teapot_vertices, teapot_indices);
	scene_update_mesh(teapot_vertices, teapot_indices, &teapot_mesh);
	scene_update_mesh_normals(teapot_vertices, &teapot_mesh.normals);

	// update teacup mesh
	sub_count = glm::clamp(8 + subdivision_delta, 2, 16);
	teacup.tesselate(sub_count, sub_count, teacup_vertices, teacup_indices);
	scene_update_mesh(teacup_vertices, teacup_indices, &teacup_mesh);
	scene_update_mesh_normals(teacup_vertices, &teacup_mesh.normals);

	// update teaspoon mesh
	sub_count = glm::clamp(8 + subdivision_delta, 2, 16);
	teaspoon.tesselate(sub_count, sub_count, teaspoon_vertices, teaspoon_indices);
	scene_update_mesh(teaspoon_vertices, teaspoon_indices, &teaspoon_mesh);
	scene_update_mesh_normals(teaspoon_vertices, &teaspoon_mesh.normals);

	// update sphere mesh
	sub_count = glm::clamp(3 + subdivision_delta, 0, 4);
	sphere.tesselate(sub_count, sphere_vertices, sphere_indices);
	scene_update_mesh(sphere_vertices, sphere_indices, &sphere_mesh);
	scene_update_mesh_normals(sphere_vertices, &sphere_mesh.normals);
}
