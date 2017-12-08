/**
 * @file sceneloader.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "sceneloader.h"
#include "entity.h"
#include "mesh.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

SceneLoader::SceneLoader(bool logger, bool verbose)
{
	if (logger) {
		if (verbose) {
			Assimp::DefaultLogger::create(nullptr, Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT);
		} else {
			Assimp::DefaultLogger::create(nullptr, Assimp::Logger::NORMAL, aiDefaultLogStream_STDOUT);
		}
	}
}

SceneLoader::~SceneLoader()
{
	Assimp::DefaultLogger::kill();
}

static bool loadMeshesFromNode(const aiScene* scene, aiMatrix4x4 current_transformation, const aiNode* node, Entity* entity)
{
	aiMatrix4x4 node_transformation = current_transformation * node->mTransformation;

	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		const aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh::PrimitiveType primitive_type;

		switch (ai_mesh->mPrimitiveTypes) {
			case aiPrimitiveType_POINT: primitive_type = Mesh::PrimitiveType::Point; break;
			case aiPrimitiveType_LINE: primitive_type = Mesh::PrimitiveType::Line; break;
			case aiPrimitiveType_TRIANGLE: primitive_type = Mesh::PrimitiveType::Triangle; break;
			default:
				continue; // unsupported primitive type; skip mesh
		}

		Mesh* mesh = new Mesh(
			primitive_type,
			ai_mesh->mNumVertices,
			!!ai_mesh->mNormals,
			!!ai_mesh->mTangents,
			!!ai_mesh->mBitangents,
			!!ai_mesh->mColors[0]
		);

		// load mesh vertex data
		for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
			aiVector3D ai_vertex = ai_mesh->mVertices[i];

			// apply node transformation
			ai_vertex *= node_transformation;

			mesh->vertex_data.insert(mesh->vertex_data.end(), { ai_vertex.x, ai_vertex.y, ai_vertex.z });

			if (ai_mesh->mNormals) {
				const aiVector3D& ai_normal = ai_mesh->mNormals[i];
				mesh->vertex_data.insert(mesh->vertex_data.end(), { ai_normal.x, ai_normal.y, ai_normal.z });
			}

			if (ai_mesh->mTangents) {
				const aiVector3D& ai_tangent = ai_mesh->mTangents[i];
				mesh->vertex_data.insert(mesh->vertex_data.end(), { ai_tangent.x, ai_tangent.y, ai_tangent.z });
			}

			if (ai_mesh->mBitangents) {
				const aiVector3D& ai_bitangent = ai_mesh->mBitangents[i];
				mesh->vertex_data.insert(mesh->vertex_data.end(), { ai_bitangent.x, ai_bitangent.y, ai_bitangent.z });
			}

			if (ai_mesh->mColors[0]) {
				const aiColor4D& ai_color = ai_mesh->mColors[0][i];
				mesh->vertex_data.insert(mesh->vertex_data.end(), { ai_color.r, ai_color.g, ai_color.b, ai_color.a });
			}
		}

		// load mesh index
		mesh->index_data.reserve(ai_mesh->mNumFaces * (unsigned int)primitive_type);
		for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
			const aiFace& ai_face = ai_mesh->mFaces[i];

			if (ai_face.mNumIndices != (unsigned int)primitive_type)
				continue;

			mesh->index_data.insert(mesh->index_data.end(), ai_face.mIndices, ai_face.mIndices + ai_face.mNumIndices);
		}

		// store mesh
		entity->meshes.push_back(mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		loadMeshesFromNode(scene, node_transformation, node->mChildren[i], entity);
	}

	return true;
}

Entity* SceneLoader::createEntityFromFile(const std::string& name, const std::string& filename)
{
	bool ret;
	Assimp::Importer importer;
	const aiScene* scene;

	scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene) {
		Assimp::DefaultLogger::get()->error(importer.GetErrorString());
		return nullptr;
	}

	Entity* entity = new Entity(name);
	ret = loadMeshesFromNode(scene, aiMatrix4x4(), scene->mRootNode, entity);
	if (!ret) {
		delete entity;
		return nullptr;
	}

	return entity;
}
