/**
 * @file testscene.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "testscene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
#define GLM_ENABLE_EXPERIMENTAL // Because string_cast.hpp includes dual_quaternion.hpp
#include <glm/gtx/string_cast.hpp>

#include "bezier.h"
#include "teaset.h"
#include "sphere.h"
#include "shape.h"
#include "gldebug.h"
#include "glhelpers.h"
#include "vertex_traits.h"

static bool ready = 0;
static int width = 0;
static int height = 0;
static unsigned int tick = 0;
static bool render_normals = false;

struct shader_program_t {
	GLuint program = 0;
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	std::map<std::string, GLint> uniform_location;
	std::map<std::string, GLint> attribute_location;
	std::map<std::string, GLint> fragdata_location;

	struct sampler_info_t {
		GLint location = -1;
		GLint unit = -1;
	};
	std::map<std::string, sampler_info_t> sampler_info;

	GLint uniform(const std::string& name) const { return uniform_location.at(name); }
	bool has_uniform(const std::string& name) const { return uniform_location.count(name) > 0; }

	GLint attribute(const std::string& name) const { return attribute_location.at(name); }
	bool has_attribute(const std::string& name) const { return attribute_location.count(name) > 0; }

	GLint sampler(const std::string& name) const { return sampler_info.at(name).location; }
	GLuint sampler_unit(const std::string& name) const { return static_cast<GLuint>(sampler_info.at(name).unit); }
	bool has_sampler(const std::string& name) const { return sampler_info.count(name) > 0; }

	GLint fragdata(const std::string& name) const { return fragdata_location.at(name); }
	bool has_fragdata(const std::string& name) const { return fragdata_location.count(name) > 0; }
};

struct vertex_t {
	glm::vec3 position;
	glm::vec3 normal;
};

struct textured_vertex_t {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

struct normals_t {
	GLuint vao = 0;

	GLuint vbo = 0;
	GLuint vbo_binding = 0;
	GLsizei vertex_count = 0;
};

struct texture_unit_t {
	GLuint unit = 0;
	GLuint texture = 0;
};

struct mesh_t {
	GLuint vao = 0;

	GLuint vbo = 0;
	GLuint vbo_binding = 0;
	GLsizei vertex_count = 0;

	GLuint ibo = 0;
	GLsizei index_count = 0;

	normals_t normals;

	std::vector<texture_unit_t> textures;

	const shader_program_t* shader = nullptr;
};

// Shader programs
static shader_program_t simple_shader;
static shader_program_t textured_shader;

// Cube mesh
static Cube cube;
static std::vector<textured_vertex_t> cube_vertices;
static std::vector<unsigned int> cube_indices;
static mesh_t cube_mesh;

// Octahedron mesh
static Octahedron octahedron;
static std::vector<vertex_t> octahedron_vertices;
static std::vector<unsigned int> octahedron_indices;
static mesh_t octahedron_mesh;

// Bezier surface mesh
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

// Helper function declarations
template<typename VertexType>
static void scene_update_mesh(
	const std::vector<VertexType>& vertices,
	const std::vector<unsigned int>& indices,
	mesh_t* mesh
);
template<typename VertexType>
static void scene_update_mesh_normals(
	const std::vector<VertexType>& vertices,
	normals_t* normals
);
static void scene_unload_shader_program(shader_program_t* shader_program);


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

	// Multisampling
	glEnable(GL_MULTISAMPLE);
	GLint msaa = 0;
	glGetIntegerv(GL_SAMPLES, &msaa);
	if (msaa > 0) {
		std::printf("MSAAx%d enabled\n", msaa);
	} else {
		std::printf("MSAA disabled\n");
	}

	// Debugging
	glDebugMessageCallback(&cortex_gldebug_callback, NULL);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	std::printf("Debug enabled\n");

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Back face culling
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

	source.resize(st.st_size);
	r = fread(source.data(), st.st_size, 1, f);
	if (r != 1) {
		fprintf(stderr, "Failed to read %s\n", filename.c_str());
		goto error;
	}

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
	std::string info_log;
	GLint compile_status = GL_FALSE;

	r = scene_read_shader(filename, source);
	if (r)
		goto error;

	shader = glCreateShader(shader_type);
	if (!shader) {
		fprintf(stderr, "glCreateShader() failed\n");
		goto error;
	}

	// Compile shader
	source_ptr = source.c_str();
	glShaderSource(shader, 1, &source_ptr, NULL);
	glCompileShader(shader);

	// Retrieve shader info log
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetShaderInfoLog(shader, info_log.size(), NULL, info_log.data());
	}

	// Check shader compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		fprintf(stderr, "Failed to compile shader:\n");

		GLint shader_source_len;
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &shader_source_len);
		std::string shader_source(shader_source_len, 0);
		glGetShaderSource(shader, shader_source.size(), NULL, shader_source.data());
		fprintf(stderr, "%s\n", shader_source.data());

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

static int scene_load_shader_program(const std::string& vertex_shader_file, const std::string& fragment_shader_file, shader_program_t* shader_program)
{
	int r;
	GLint link_status = GL_FALSE;
	GLint validate_status = GL_FALSE;
	GLint info_log_len;
	std::string info_log;
	GLint uniform_count;
	GLint uniform_max_length;
	GLint attribute_count;
	GLint attribute_max_length;
	GLint output_count;
	GLint output_max_length;

	cortex_gldebug_msg("Loading shader program: %s; %s",
		vertex_shader_file.c_str(),
		fragment_shader_file.c_str()
	);

	shader_program->program = glCreateProgram();
	if (!shader_program->program) {
		fprintf(stderr, "glCreateProgram() failed\n");
		r = -1;
		goto error;
	}

	shader_program->vertex_shader = scene_load_shader(vertex_shader_file, GL_VERTEX_SHADER);
	if (!shader_program->vertex_shader) {
		fprintf(stderr, "Failed to load vertex shader: %s\n", vertex_shader_file.c_str());
		r = -2;
		goto error;
	}

	shader_program->fragment_shader = scene_load_shader(fragment_shader_file, GL_FRAGMENT_SHADER);
	if (!shader_program->fragment_shader) {
		fprintf(stderr, "Failed to load fragment shader: %s\n", fragment_shader_file.c_str());
		r = -3;
		goto error;
	}

	// Add shaders to program
	glAttachShader(shader_program->program, shader_program->vertex_shader);
	glAttachShader(shader_program->program, shader_program->fragment_shader);

	// Link program
	glLinkProgram(shader_program->program);
	info_log_len = 0;
	info_log.clear();
	glGetProgramiv(shader_program->program, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetProgramInfoLog(shader_program->program, info_log.size(), NULL, info_log.data());
	}

	glGetProgramiv(shader_program->program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		fprintf(stderr, "Error linking shader program:\n%s\n", info_log.data());
		r = -4;
		goto error;
	}
	if (!info_log.empty()) {
		printf("Shader program log:\n%s\n", info_log.data());
	}

	// Validate program
	glValidateProgram(shader_program->program);
	info_log_len = 0;
	info_log.clear();
	glGetProgramiv(shader_program->program, GL_INFO_LOG_LENGTH, &info_log_len);
	if (info_log_len > 1) {
		info_log.resize(info_log_len);
		glGetProgramInfoLog(shader_program->program, info_log.size(), NULL, info_log.data());
	}

	glGetProgramiv(shader_program->program, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status != GL_TRUE) {
		fprintf(stderr, "Invalid shader program:\n%s\n", info_log.data());
		r = -5;
		goto error;
	}

	// Lookup all uniforms
	uniform_count = 0;
	uniform_max_length = 0;
	glGetProgramiv(shader_program->program, GL_ACTIVE_UNIFORMS, &uniform_count);
	glGetProgramiv(shader_program->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_max_length);
	for (int i = 0; i < uniform_count; ++i) {
		std::string uniform_name(uniform_max_length, 0);
		GLint uniform_size = 0;
		GLenum uniform_type = 0;
		GLint uniform_location;
		glGetActiveUniform(shader_program->program, i, uniform_name.size(), NULL, &uniform_size, &uniform_type, uniform_name.data());
		uniform_location = glGetUniformLocation(shader_program->program, uniform_name.data());

		if (glUniformTypeIsSampler(uniform_type)) {
			GLint unit = 0;
			glGetUniformiv(shader_program->program, uniform_location, &unit);
			shader_program->sampler_info[uniform_name.data()] = { uniform_location, unit };
			cortex_gldebug_sampler(uniform_name.data(), uniform_size, uniform_type, uniform_location, unit);
		} else {
			shader_program->uniform_location[uniform_name.data()] = uniform_location;
			cortex_gldebug_uniform(uniform_name.data(), uniform_size, uniform_type, uniform_location);
		}
	}

	// Lookup all attributes
	attribute_count = 0;
	attribute_max_length = 0;
	glGetProgramiv(shader_program->program, GL_ACTIVE_ATTRIBUTES, &attribute_count);
	glGetProgramiv(shader_program->program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribute_max_length);
	for (int i = 0; i < attribute_count; ++i) {
		std::string attribute_name(attribute_max_length, 0);
		GLint attribute_size = 0;
		GLenum attribute_type = 0;
		glGetActiveAttrib(shader_program->program, i, attribute_name.size(), NULL, &attribute_size, &attribute_type, attribute_name.data());
		shader_program->attribute_location[attribute_name.data()] = glGetAttribLocation(shader_program->program, attribute_name.data());
		cortex_gldebug_attribute(attribute_name.data(), attribute_size, attribute_type, shader_program->attribute_location[attribute_name.data()]);
	}

	// Lookup all program outputs
	output_count = 0;
	output_max_length = 0;
	glGetProgramInterfaceiv(shader_program->program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &output_count);
	glGetProgramInterfaceiv(shader_program->program, GL_PROGRAM_OUTPUT, GL_MAX_NAME_LENGTH, &output_max_length);
	for (int i = 0; i < output_count; ++i) {
		std::string output_name(output_max_length, 0);
		GLsizei output_name_len = 0;
		GLint output_location;
		const GLenum output_props[] = { GL_TYPE, GL_ARRAY_SIZE };
		GLint output_values[2] = { 0, 0 };
		glGetProgramResourceName(shader_program->program, GL_PROGRAM_OUTPUT, i, output_name.size(), &output_name_len, output_name.data());
		glGetProgramResourceiv(shader_program->program, GL_PROGRAM_OUTPUT, i, 2, output_props, 2, nullptr, output_values);
		output_location = glGetProgramResourceLocation(shader_program->program, GL_PROGRAM_OUTPUT, output_name.data());
		shader_program->fragdata_location[output_name.data()] = output_location;
		cortex_gldebug_fragdata(output_name.data(), output_values[1], static_cast<GLenum>(output_values[0]), output_location);
	}

	cortex_gldebug_msg("Shader program %u loaded", shader_program->program);

	r = 0;
	goto exit;

error:
	scene_unload_shader_program(shader_program);

exit:
	return r;
}

template<typename VertexType>
static void scene_load_mesh(
	const std::vector<VertexType>& vertices,
	const std::vector<unsigned int>& indices,
	const shader_program_t* shader,
	mesh_t* mesh)
{
	// VAO layout:
	// - VBO #0 for interleaved vertex data (position, normal, texcoord)
	// - IBO for element indexes

	// Create vertex array object and vertex/index buffer objects
	glCreateVertexArrays(1, &mesh->vao);
	glCreateBuffers(1, &mesh->vbo);
	glCreateBuffers(1, &mesh->ibo);

	// Bind buffer objects to vertex array object
	glVertexArrayVertexBuffer(mesh->vao, mesh->vbo_binding, mesh->vbo, 0, sizeof(VertexType));
	glVertexArrayElementBuffer(mesh->vao, mesh->ibo);

	// Setup format and binding for vertex position
	GLuint pos_loc = shader->attribute("v_position");
	glEnableVertexArrayAttrib(mesh->vao, pos_loc);
	glVertexArrayAttribBinding(mesh->vao, pos_loc, mesh->vbo_binding);
	glVertexArrayAttribFormat(mesh->vao, pos_loc, 3, GL_FLOAT, GL_FALSE, 0);

	// Setup format and binding for vertex normal
	if constexpr (detail::has_normal<VertexType>::value) {
		if (shader->has_attribute("v_normal")) {
			GLuint norm_loc = shader->attribute("v_normal");
			glEnableVertexArrayAttrib(mesh->vao, norm_loc);
			glVertexArrayAttribBinding(mesh->vao, norm_loc, mesh->vbo_binding);
			glVertexArrayAttribFormat(mesh->vao, norm_loc, 3, GL_FLOAT, GL_FALSE, offsetof(VertexType, normal));
		}
	}

	// Setup format and binding for texture coordinates
	if constexpr (detail::has_texcoord<VertexType>::value) {
		if (shader->has_attribute("v_texcoord")) {
			GLuint tc_loc = shader->attribute("v_texcoord");
			glEnableVertexArrayAttrib(mesh->vao, tc_loc);
			glVertexArrayAttribBinding(mesh->vao, tc_loc, mesh->vbo_binding);
			glVertexArrayAttribFormat(mesh->vao, tc_loc, 2, GL_FLOAT, GL_FALSE, offsetof(VertexType, texcoord));
		}
	}

	mesh->shader = shader;

	// Load data
	scene_update_mesh(vertices, indices, mesh);
}

template<typename VertexType>
static void scene_update_mesh(
	const std::vector<VertexType>& vertices,
	const std::vector<unsigned int>& indices,
	mesh_t* mesh)
{
	// Update existing vertex buffer object
	glNamedBufferData(mesh->vbo, vertices.size() * sizeof(VertexType), vertices.data(), GL_DYNAMIC_DRAW);
	mesh->vertex_count = vertices.size();

	// Update existing index buffer object
	glNamedBufferData(mesh->ibo, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
	mesh->index_count = indices.size();

	printf("%s(); vao=%u; vbo=%u[%zu]; ibo=%u[%zu]\n", __FUNCTION__, mesh->vao, mesh->vbo, vertices.size(), mesh->ibo, indices.size());
}

template<typename VertexType>
static void scene_load_mesh_normals(
	const std::vector<VertexType>& vertices,
	const shader_program_t* shader,
	normals_t* normals
)
{
	// VAO layout:
	// - VBO #0 for position
	// - No IBO

	// Create vertex array object and vertex buffer object
	glCreateVertexArrays(1, &normals->vao);
	glCreateBuffers(1, &normals->vbo);

	// Bind vertex buffer to vertex array object
	glVertexArrayVertexBuffer(normals->vao, normals->vbo_binding, normals->vbo, 0, sizeof(glm::vec3));

	// Setup format and binding for vertex data
	GLuint pos_loc = shader->attribute("v_position");
	glEnableVertexArrayAttrib(normals->vao, pos_loc);
	glVertexArrayAttribBinding(normals->vao, pos_loc, normals->vbo_binding);
	glVertexArrayAttribFormat(normals->vao, pos_loc, 3, GL_FLOAT, GL_FALSE, 0);

	// Load data
	scene_update_mesh_normals(vertices, normals);
}

template<typename VertexType>
static void scene_update_mesh_normals(
	const std::vector<VertexType>& vertices,
	normals_t* normals
)
{
	// Generate vertices representing normal lines
	using line_type = std::pair<glm::vec3,glm::vec3>;
	std::vector<line_type> normal_lines;
	normal_lines.reserve(vertices.size());
	for (auto&& vertex : vertices) {
		normal_lines.emplace_back(
			vertex.position,
			vertex.position + (glm::normalize(vertex.normal) * 0.3f)
		);
	}

	// Update existing vertex buffer object
	glNamedBufferData(normals->vbo, normal_lines.size() * sizeof(line_type), normal_lines.data(), GL_DYNAMIC_DRAW);
	normals->vertex_count = normal_lines.size() * 2; // two vertices per line

	printf("%s(); vao=%u; vbo=%u[%zu]\n", __FUNCTION__, normals->vao, normals->vbo, normal_lines.size());
}

static GLuint scene_load_texture(const std::string& filename)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		fprintf(stderr, "Failed to load texture: %s\n", filename.c_str());
		return 0;
	}

	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(data);

	printf("%s(); tex=%u; %dx%d; channels=%d\n", __FUNCTION__, texture, width, height, channels);
	return texture;
}

int scene_load_resources(void)
{
	int r;

	// Load shaders
	r = scene_load_shader_program("test/simple.vert.glsl", "test/simple.frag.glsl", &simple_shader);
	if (r) {
		fprintf(stderr, "Failed to load shader program\n");
		return r;
	}

	r = scene_load_shader_program("test/textured.vert.glsl", "test/textured.frag.glsl", &textured_shader);
	if (r) {
		fprintf(stderr, "Failed to load textured shader program\n");
		return r;
	}

	// Load cube mesh
	cube.tessellate(cube_vertices, cube_indices);
	scene_load_mesh(cube_vertices, cube_indices, &textured_shader, &cube_mesh);
	scene_load_mesh_normals(cube_vertices, &textured_shader, &cube_mesh.normals);
	cube_mesh.textures.push_back({
		textured_shader.sampler_unit("material_diffuse"),
		scene_load_texture("test/data/container2.png")
	});
	cube_mesh.textures.push_back({
		textured_shader.sampler_unit("material_specular"),
		scene_load_texture("test/data/container2_specular.png")
	});

	// Load octahedron mesh
	octahedron.tessellate(octahedron_vertices, octahedron_indices);
	scene_load_mesh(octahedron_vertices, octahedron_indices, &simple_shader, &octahedron_mesh);
	scene_load_mesh_normals(octahedron_vertices, &simple_shader, &octahedron_mesh.normals);

	// Load bezier surface mesh
	bezier_surface.tessellate(16, 16, bezier_surface_vertices, bezier_surface_indices);
	scene_load_mesh(bezier_surface_vertices, bezier_surface_indices, &simple_shader, &bezier_surface_mesh);
	scene_load_mesh_normals(bezier_surface_vertices, &simple_shader, &bezier_surface_mesh.normals);

	// Load teapot mesh
	teapot.tessellate(12, 12, teapot_vertices, teapot_indices);
	scene_load_mesh(teapot_vertices, teapot_indices, &simple_shader, &teapot_mesh);
	scene_load_mesh_normals(teapot_vertices, &simple_shader, &teapot_mesh.normals);

	// Load teacup mesh
	teacup.tessellate(8, 8, teacup_vertices, teacup_indices);
	scene_load_mesh(teacup_vertices, teacup_indices, &simple_shader, &teacup_mesh);
	scene_load_mesh_normals(teacup_vertices, &simple_shader, &teacup_mesh.normals);

	// Load teaspoon mesh
	teaspoon.tessellate(8, 8, teaspoon_vertices, teaspoon_indices);
	scene_load_mesh(teaspoon_vertices, teaspoon_indices, &simple_shader, &teaspoon_mesh);
	scene_load_mesh_normals(teaspoon_vertices, &simple_shader, &teaspoon_mesh.normals);

	// Load sphere mesh
	sphere.tessellate(3, sphere_vertices, sphere_indices);
	scene_load_mesh(sphere_vertices, sphere_indices, &simple_shader, &sphere_mesh);
	scene_load_mesh_normals(sphere_vertices, &simple_shader, &sphere_mesh.normals);

	return 0;
}

static void scene_unload_shader_program(shader_program_t* shader_program)
{
	shader_program->uniform_location.clear();
	shader_program->attribute_location.clear();

	if (shader_program->vertex_shader) {
		glDeleteShader(shader_program->vertex_shader);
	}
	if (shader_program->fragment_shader) {
		glDeleteShader(shader_program->fragment_shader);
	}
	if (shader_program->program) {
		glDeleteProgram(shader_program->program);
	}
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

	for (const auto& t : mesh->textures) {
		glDeleteTextures(1, &t.texture);
	}
	mesh->textures.clear();
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

	scene_unload_shader_program(&simple_shader);
	scene_unload_shader_program(&textured_shader);
}

void scene_update(void)
{
	++tick;
}

void scene_render(enum scene_demo_t scene_demo)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	// Determine current mesh and shader program
	const mesh_t* current_mesh = nullptr;
	const shader_program_t* current_shader = nullptr;
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
	current_shader = current_mesh->shader;

	// Uniform matrices
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));
	glm::mat4 m_model_rotate_x = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 m_model_rotate_y = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_model_rotate_z = glm::rotate(glm::mat4(1.0f), glm::radians((float)tick / 4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m_model = m_model_rotate_z * m_model_rotate_y * m_model_rotate_x;
	glm::mat4 m_modelview = m_view * m_model;
	glm::mat4 m_mvp = m_projection * m_modelview;
	glm::mat3 m_normal = glm::inverseTranspose(glm::mat3(m_modelview));

	glProgramUniformMatrix4fv(current_shader->program, current_shader->uniform("m_mvp"), 1, GL_FALSE, glm::value_ptr(m_mvp));
	glProgramUniformMatrix4fv(current_shader->program, current_shader->uniform("m_modelview"), 1, GL_FALSE, glm::value_ptr(m_modelview));
	glProgramUniformMatrix3fv(current_shader->program, current_shader->uniform("m_normal"), 1, GL_FALSE, glm::value_ptr(m_normal));

	// Uniform light parameters
	glm::vec4 light_position = glm::vec4(15.0f, 15.0f, 15.0f, 1.0f);
	glm::vec3 light_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 light_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

	if (current_shader->has_uniform("light.position")) {
		glProgramUniform4fv(current_shader->program, current_shader->uniform("light.position"), 1, glm::value_ptr(light_position));
	}
	if (current_shader->has_uniform("light.ambient")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("light.ambient"), 1, glm::value_ptr(light_ambient));
	}
	if (current_shader->has_uniform("light.diffuse")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("light.diffuse"), 1, glm::value_ptr(light_diffuse));
	}
	if (current_shader->has_uniform("light.specular")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("light.specular"), 1, glm::value_ptr(light_specular));
	}

	// Uniform material parameters
	glm::vec3 material_ambient = glm::vec3(0.2f, 0.0f, 0.0f);
	glm::vec3 material_diffuse = glm::vec3(0.8f, 0.0f, 0.0f);
	glm::vec3 material_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float material_shininess = 25;

	if (current_shader->has_uniform("material.ambient")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("material.ambient"), 1, glm::value_ptr(material_ambient));
	}
	if (current_shader->has_uniform("material.diffuse")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("material.diffuse"), 1, glm::value_ptr(material_diffuse));
	}
	if (current_shader->has_uniform("material.specular")) {
		glProgramUniform3fv(current_shader->program, current_shader->uniform("material.specular"), 1, glm::value_ptr(material_specular));
	}
	if (current_shader->has_uniform("material.shininess")) {
		glProgramUniform1f(current_shader->program, current_shader->uniform("material.shininess"), material_shininess);
	}

	// Bind textures
	for (const auto& t : current_mesh->textures) {
		glBindTextureUnit(t.unit, t.texture);
	}

	// Render current mesh
	glUseProgram(current_shader->program);
	glBindVertexArray(current_mesh->vao);
	glDrawElements(GL_TRIANGLES, current_mesh->index_count, GL_UNSIGNED_INT, 0);

	if (render_normals && current_mesh->normals.vao) {
		const shader_program_t* normal_shader = &simple_shader;

		// Uniform matrices for normal lines
		glProgramUniformMatrix4fv(normal_shader->program, normal_shader->uniform("m_mvp"), 1, GL_FALSE, glm::value_ptr(m_mvp));
		glProgramUniformMatrix4fv(normal_shader->program, normal_shader->uniform("m_modelview"), 1, GL_FALSE, glm::value_ptr(m_modelview));
		glProgramUniformMatrix3fv(normal_shader->program, normal_shader->uniform("m_normal"), 1, GL_FALSE, glm::value_ptr(m_normal));

		// Update uniform light parameters for normal lines
		light_ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		light_diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glProgramUniform3fv(normal_shader->program, normal_shader->uniform("light.ambient"), 1, glm::value_ptr(light_ambient));
		glProgramUniform3fv(normal_shader->program, normal_shader->uniform("light.diffuse"), 1, glm::value_ptr(light_diffuse));

		// Update uniform material parameters for normal lines
		material_ambient = glm::vec3(1.0f, 1.0f, 1.0f);
		material_diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glProgramUniform3fv(normal_shader->program, normal_shader->uniform("material.ambient"), 1, glm::value_ptr(material_ambient));
		glProgramUniform3fv(normal_shader->program, normal_shader->uniform("material.diffuse"), 1, glm::value_ptr(material_diffuse));

		// Render current normals
		glUseProgram(normal_shader->program);
		glBindVertexArray(current_mesh->normals.vao);
		glDrawArrays(GL_LINES, 0, current_mesh->normals.vertex_count);
	}

	// Cleanup
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

	// Clear data
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

	// Update bezier surface mesh
	sub_count = glm::clamp(16 + subdivision_delta, 2, 24);
	bezier_surface.tessellate(sub_count, sub_count, bezier_surface_vertices, bezier_surface_indices);
	scene_update_mesh(bezier_surface_vertices, bezier_surface_indices, &bezier_surface_mesh);
	scene_update_mesh_normals(bezier_surface_vertices, &bezier_surface_mesh.normals);

	// Update teapot mesh
	sub_count = glm::clamp(12 + subdivision_delta, 2, 24);
	teapot.tessellate(sub_count, sub_count, teapot_vertices, teapot_indices);
	scene_update_mesh(teapot_vertices, teapot_indices, &teapot_mesh);
	scene_update_mesh_normals(teapot_vertices, &teapot_mesh.normals);

	// Update teacup mesh
	sub_count = glm::clamp(8 + subdivision_delta, 2, 16);
	teacup.tessellate(sub_count, sub_count, teacup_vertices, teacup_indices);
	scene_update_mesh(teacup_vertices, teacup_indices, &teacup_mesh);
	scene_update_mesh_normals(teacup_vertices, &teacup_mesh.normals);

	// Update teaspoon mesh
	sub_count = glm::clamp(8 + subdivision_delta, 2, 16);
	teaspoon.tessellate(sub_count, sub_count, teaspoon_vertices, teaspoon_indices);
	scene_update_mesh(teaspoon_vertices, teaspoon_indices, &teaspoon_mesh);
	scene_update_mesh_normals(teaspoon_vertices, &teaspoon_mesh.normals);

	// Update sphere mesh
	sub_count = glm::clamp(3 + subdivision_delta, 0, 4);
	sphere.tessellate(sub_count, sphere_vertices, sphere_indices);
	scene_update_mesh(sphere_vertices, sphere_indices, &sphere_mesh);
	scene_update_mesh_normals(sphere_vertices, &sphere_mesh.normals);
}
