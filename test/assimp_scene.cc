/**
 * @file assimp_scene.cc
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "assimp_scene.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cfloat>

#include <map>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gldebug.h"
#include "glhelpers.h"

static bool ready = false;
static int width = 0;
static int height = 0;

// Camera state
static glm::quat camera_orientation(1.0f, 0.0f, 0.0f, 0.0f);
static float camera_radius = 2.0f;
static float camera_radius_initial = 2.0f;
static glm::vec3 camera_target(0.0f, 0.0f, 0.0f);

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
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 texcoord;
};

struct material_t {
	glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float shininess = 25.0f;
};

struct mesh_t {
	GLuint vao = 0;

	GLuint vbo = 0;
	GLuint vbo_binding = 0;
	GLsizei vertex_count = 0;

	GLuint ibo = 0;
	GLsizei index_count = 0;

	material_t material;

	const shader_program_t* shader = nullptr;
};

// Shader programs
static shader_program_t simple_shader;

// Scene meshes
static std::vector<mesh_t> meshes;

// Helper function declarations
static void scene_update_mesh(
	const std::vector<vertex_t>& vertices,
	const std::vector<unsigned int>& indices,
	mesh_t* mesh
);
static void scene_unload_shader_program(shader_program_t* shader_program);


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
	if (r) {
		goto error;
	}

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

static void scene_load_mesh(
	const std::vector<vertex_t>& vertices,
	const std::vector<unsigned int>& indices,
	const shader_program_t* shader,
	mesh_t* mesh)
{
	// VAO layout:
	// - VBO #0 for interleaved vertex data
	// - IBO for element indexes

	// Create vertex array object and vertex/index buffer objects
	glCreateVertexArrays(1, &mesh->vao);
	glCreateBuffers(1, &mesh->vbo);
	glCreateBuffers(1, &mesh->ibo);

	// Bind buffer objects to vertex array object
	glVertexArrayVertexBuffer(mesh->vao, mesh->vbo_binding, mesh->vbo, 0, sizeof(vertex_t));
	glVertexArrayElementBuffer(mesh->vao, mesh->ibo);

	// Setup format and binding for vertex position
	GLuint pos_loc = shader->attribute("v_position");
	glEnableVertexArrayAttrib(mesh->vao, pos_loc);
	glVertexArrayAttribBinding(mesh->vao, pos_loc, mesh->vbo_binding);
	glVertexArrayAttribFormat(mesh->vao, pos_loc, 3, GL_FLOAT, GL_FALSE, 0);

	// Setup format and binding for vertex normal
	if (shader->has_attribute("v_normal")) {
		GLuint norm_loc = shader->attribute("v_normal");
		glEnableVertexArrayAttrib(mesh->vao, norm_loc);
		glVertexArrayAttribBinding(mesh->vao, norm_loc, mesh->vbo_binding);
		glVertexArrayAttribFormat(mesh->vao, norm_loc, 3, GL_FLOAT, GL_FALSE, offsetof(vertex_t, normal));
	}

	// Setup format and binding for vertex tangent
	if (shader->has_attribute("v_tangent")) {
		GLuint tan_loc = shader->attribute("v_tangent");
		glEnableVertexArrayAttrib(mesh->vao, tan_loc);
		glVertexArrayAttribBinding(mesh->vao, tan_loc, mesh->vbo_binding);
		glVertexArrayAttribFormat(mesh->vao, tan_loc, 3, GL_FLOAT, GL_FALSE, offsetof(vertex_t, tangent));
	}

	// Setup format and binding for vertex bitangent
	if (shader->has_attribute("v_bitangent")) {
		GLuint bitan_loc = shader->attribute("v_bitangent");
		glEnableVertexArrayAttrib(mesh->vao, bitan_loc);
		glVertexArrayAttribBinding(mesh->vao, bitan_loc, mesh->vbo_binding);
		glVertexArrayAttribFormat(mesh->vao, bitan_loc, 3, GL_FLOAT, GL_FALSE, offsetof(vertex_t, bitangent));
	}

	// Setup format and binding for texture coordinates
	if (shader->has_attribute("v_texcoord")) {
		GLuint tc_loc = shader->attribute("v_texcoord");
		glEnableVertexArrayAttrib(mesh->vao, tc_loc);
		glVertexArrayAttribBinding(mesh->vao, tc_loc, mesh->vbo_binding);
		glVertexArrayAttribFormat(mesh->vao, tc_loc, 2, GL_FLOAT, GL_FALSE, offsetof(vertex_t, texcoord));
	}

	mesh->shader = shader;

	// Load data
	scene_update_mesh(vertices, indices, mesh);
}

static void scene_update_mesh(
	const std::vector<vertex_t>& vertices,
	const std::vector<unsigned int>& indices,
	mesh_t* mesh)
{
	// Update existing vertex buffer object
	glNamedBufferData(mesh->vbo, vertices.size() * sizeof(vertex_t), vertices.data(), GL_STATIC_DRAW);
	mesh->vertex_count = vertices.size();

	// Update existing index buffer object
	glNamedBufferData(mesh->ibo, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	mesh->index_count = indices.size();

	printf("%s(); vao=%u; vbo=%u[%zu]; ibo=%u[%zu]\n", __FUNCTION__, mesh->vao, mesh->vbo, vertices.size(), mesh->ibo, indices.size());
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

static void scene_load_node_meshes(
	const aiScene* ai_scene,
	const aiNode* node,
	const glm::mat4& parent_transform,
	const shader_program_t* shader,
	std::vector<mesh_t>& meshes,
	glm::vec3& aabb_min,
	glm::vec3& aabb_max
)
{
	// aiMatrix4x4 is row-major but glm::mat4 is column-major
	glm::mat4 node_transform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));

	// Combine with parent transform for object space transform
	glm::mat4 obj_transform = parent_transform * node_transform;
	glm::mat3 tangent_transform = glm::mat3(obj_transform);
	glm::mat3 normal_transform = glm::inverseTranspose(tangent_transform);

	for (unsigned int ai_node_mesh_idx = 0;
		ai_node_mesh_idx < node->mNumMeshes;
		++ai_node_mesh_idx
	) {
		unsigned int ai_scene_mesh_idx = node->mMeshes[ai_node_mesh_idx];
		const aiMesh* ai_mesh = ai_scene->mMeshes[ai_scene_mesh_idx];

		if (ai_mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
			fprintf(stderr, "mPrimitiveTypes=0x%02X not supported; skipping mesh\n", ai_mesh->mPrimitiveTypes);
			continue;
		}

		std::vector<vertex_t> vertices;
		std::vector<unsigned int> indices;

		// Process vertices
		vertices.reserve(ai_mesh->mNumVertices);
		bool has_normals = ai_mesh->HasNormals();
		bool has_tangents = ai_mesh->HasTangentsAndBitangents();
		bool has_texcoords = ai_mesh->HasTextureCoords(0);
		for (unsigned int ai_mesh_vertex_idx = 0;
			ai_mesh_vertex_idx < ai_mesh->mNumVertices;
			++ai_mesh_vertex_idx
		) {
			vertex_t& vertex = vertices.emplace_back();

			// Transform vertex position in node to object space
			const aiVector3D& v = ai_mesh->mVertices[ai_mesh_vertex_idx];
			glm::vec4 v_pos = obj_transform * glm::vec4(v.x, v.y, v.z, 1.0f);
			vertex.position = glm::vec3(v_pos);

			// Transform normal vector in node to object space
			if (has_normals) {
				const aiVector3D& n = ai_mesh->mNormals[ai_mesh_vertex_idx];
				vertex.normal = glm::normalize(normal_transform * glm::vec3(n.x, n.y, n.z));
			}

			// Transform tangent and bitangent vectors in node to object space
			if (has_tangents) {
				const aiVector3D& t = ai_mesh->mTangents[ai_mesh_vertex_idx];
				vertex.tangent = glm::normalize(tangent_transform * glm::vec3(t.x, t.y, t.z));

				const aiVector3D& b = ai_mesh->mBitangents[ai_mesh_vertex_idx];
				vertex.bitangent = glm::normalize(tangent_transform * glm::vec3(b.x, b.y, b.z));
			}

			// Load texture coordinates
			if (has_texcoords) {
				const aiVector3D& uv = ai_mesh->mTextureCoords[0][ai_mesh_vertex_idx];
				vertex.texcoord = glm::vec2(uv.x, uv.y);
			}
		}

		// Process vertex indices
		indices.reserve(ai_mesh->mNumFaces * 3); // Assume triangles
		for (unsigned int f = 0; f < ai_mesh->mNumFaces; ++f) {
			const aiFace& face = ai_mesh->mFaces[f];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// Update bounding box
		for (const vertex_t& v : vertices) {
			aabb_min = glm::min(aabb_min, v.position);
			aabb_max = glm::max(aabb_max, v.position);
		}

		// Process material
		material_t material;
		if (ai_mesh->mMaterialIndex < ai_scene->mNumMaterials) {
			const aiMaterial* ai_mat = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
			aiColor3D color;
			float shininess = 0.0f;

			// Populate material colour properties
			if (ai_mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
				material.ambient = glm::vec3(color.r, color.g, color.b);
			}
			if (ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
				material.diffuse = glm::vec3(color.r, color.g, color.b);
			}
			if (ai_mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
				material.specular = glm::vec3(color.r, color.g, color.b);
			}
			if (ai_mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
				if (shininess > 0.0f) {
					material.shininess = shininess;
				}
			}
		}

		// Load mesh
		mesh_t& mesh = meshes.emplace_back();
		mesh.material = material;
		scene_load_mesh(vertices, indices, shader, &mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		scene_load_node_meshes(ai_scene, node->mChildren[i], obj_transform, shader, meshes, aabb_min, aabb_max);
	}
}

int scene_init(void)
{
	if (ready) {
		return 0;
	}

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

	// Clear colour buffer using opaque black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// sRGB framebuffer output
	glEnable(GL_FRAMEBUFFER_SRGB);

	ready = true;

	return 0;
}

int scene_load_resources(const char* filename)
{
	int r;
	Assimp::Importer importer;
	const aiScene* ai_scene = nullptr;
	glm::vec3 aabb_min(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 aabb_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Load shaders
	r = scene_load_shader_program(
		"test/simple.vert.glsl",
		"test/simple.frag.glsl",
		&simple_shader
	);
	if (r) {
		fprintf(stderr, "Failed to load shader program\n");
		return r;
	}

	// Load assimp scene
	ai_scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!ai_scene) {
		fprintf(stderr, "%s: %s\n", filename, importer.GetErrorString());
		r = -1;
		goto error;
	}
	scene_load_node_meshes(ai_scene, ai_scene->mRootNode, glm::mat4(1.0f), &simple_shader, meshes, aabb_min, aabb_max);

	// Update camera state according to bounding box
	if (!meshes.empty()) {
		glm::vec3 aabb_center;
		glm::vec3 aabb_extent;
		float radius;

		aabb_center = (aabb_min + aabb_max) * 0.5f;
		aabb_extent = aabb_max - aabb_min;
		radius = glm::max(glm::max(aabb_extent.x, aabb_extent.y), aabb_extent.z) * 1.5f;
		if (radius < 0.001f) {
			radius = 1.0f;
		}

		camera_target = aabb_center;
		camera_radius = radius;
		camera_radius_initial = radius;
	}

	r = 0;
	goto exit;

error:
	scene_unload_resources();

exit:
	return r;
}

void scene_unload_resources(void)
{
	for (mesh_t& mesh : meshes) {
		scene_unload_mesh(&mesh);
	}
	meshes.clear();
	scene_unload_shader_program(&simple_shader);
}

void scene_resize(int _width, int _height)
{
	printf("%s(); width=%d; height=%d\n", __FUNCTION__, _width, _height);
	width = _width;
	height = _height;
}

void scene_update(void)
{
}

void scene_render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	// Build view matrix from camera state
	glm::vec3 camera_pos = camera_target + camera_orientation * glm::vec3(0.0f, 0.0f, camera_radius);
	glm::vec3 camera_up  = camera_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), width / (float)height, 0.01f * camera_radius, 10.0f * camera_radius);
	glm::mat4 m_view = glm::lookAt(camera_pos, camera_target, camera_up);
	glm::mat4 m_modelview = m_view;
	glm::mat4 m_mvp = m_projection * m_modelview;
	glm::mat3 m_normal = glm::inverseTranspose(glm::mat3(m_modelview));

	glProgramUniformMatrix4fv(simple_shader.program, simple_shader.uniform("m_mvp"), 1, GL_FALSE, glm::value_ptr(m_mvp));
	glProgramUniformMatrix4fv(simple_shader.program, simple_shader.uniform("m_modelview"), 1, GL_FALSE, glm::value_ptr(m_modelview));
	glProgramUniformMatrix3fv(simple_shader.program, simple_shader.uniform("m_normal"), 1, GL_FALSE, glm::value_ptr(m_normal));

	// Uniform light parameters
	glm::vec4 light_position = glm::vec4(15.0f, 15.0f, 15.0f, 1.0f);
	glm::vec3 light_ambient  = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 light_diffuse  = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	glProgramUniform4fv(simple_shader.program, simple_shader.uniform("light.position"), 1, glm::value_ptr(light_position));
	glProgramUniform3fv(simple_shader.program, simple_shader.uniform("light.ambient"), 1, glm::value_ptr(light_ambient));
	glProgramUniform3fv(simple_shader.program, simple_shader.uniform("light.diffuse"), 1, glm::value_ptr(light_diffuse));
	glProgramUniform3fv(simple_shader.program, simple_shader.uniform("light.specular"), 1, glm::value_ptr(light_specular));

	// Render meshes
	glUseProgram(simple_shader.program);
	for (mesh_t& mesh : meshes) {
		glProgramUniform3fv(simple_shader.program, simple_shader.uniform("material.ambient"), 1, glm::value_ptr(mesh.material.ambient));
		glProgramUniform3fv(simple_shader.program, simple_shader.uniform("material.diffuse"), 1, glm::value_ptr(mesh.material.diffuse));
		glProgramUniform3fv(simple_shader.program, simple_shader.uniform("material.specular"), 1, glm::value_ptr(mesh.material.specular));
		glProgramUniform1f(simple_shader.program, simple_shader.uniform("material.shininess"), mesh.material.shininess);
		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
	}

	// Cleanup
	glBindVertexArray(0);
	glUseProgram(0);
}

void scene_set_wireframe(bool enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}

void scene_rotate(float delta_yaw, float delta_pitch)
{
	// Rotate around the camera's current up axis for yaw, and the camera's
	// current right axis for pitch. This keeps the rotation axes aligned with
	// the screen regardless of the camera orientation.
	glm::vec3 cam_up = camera_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cam_right = camera_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::quat yaw_rot = glm::angleAxis(glm::radians(-delta_yaw), cam_up);
	glm::quat pitch_rot = glm::angleAxis(glm::radians(-delta_pitch), cam_right);
	camera_orientation = glm::normalize(yaw_rot * pitch_rot * camera_orientation);
}

void scene_zoom(float delta)
{
	camera_radius = glm::clamp(camera_radius + delta, 0.1f * camera_radius_initial, 10.0f * camera_radius_initial);
}
