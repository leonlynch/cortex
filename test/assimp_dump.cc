/**
 * @file assimp_dump.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <set>
#include <type_traits>
#include <utility>

template <typename T>
static void print_vector(const T& v)
{
	constexpr std::size_t count = sizeof(T) / sizeof(ai_real);
	const ai_real* f = reinterpret_cast<const ai_real*>(&v);

	std::printf("{ ");
	for (std::size_t i = 0; i < count; ++i) {
		if (i) {
			std::printf(", ");
		}
		std::printf("%f", f[i]);
	}
	std::printf(" }");
}

template <typename T>
static void print_vector(const char* name, const T& v)
{
	std::printf("\t%s = ", name);
	print_vector(v);
	std::printf("\n");
}

static void print_summary(const char* display_name, const aiScene* scene)
{
	std::printf("\n%s summary:\n", display_name);

	std::printf("\tmFlags=");
	{
		bool first = true;
		for (unsigned int bit = 0x1; bit != 0; bit <<= 1) {
			if (!(scene->mFlags & bit)) {
				continue;
			}
			if (!first) {
				std::printf(",");
			}
			first = false;
			if (bit == AI_SCENE_FLAGS_INCOMPLETE) {
				std::printf("INCOMPLETE");
			} else if (bit == AI_SCENE_FLAGS_VALIDATED) {
				std::printf("VALIDATED");
			} else if (bit == AI_SCENE_FLAGS_VALIDATION_WARNING) {
				std::printf("VALIDATION_WARNING");
			} else if (bit == AI_SCENE_FLAGS_NON_VERBOSE_FORMAT) {
				std::printf("NON_VERBOSE_FORMAT");
			} else if (bit == AI_SCENE_FLAGS_TERRAIN) {
				std::printf("TERRAIN");
			} else if (bit == AI_SCENE_FLAGS_ALLOW_SHARED) {
				std::printf("ALLOW_SHARED");
			} else {
				std::printf("0x%x", bit);
			}
		}
		if (first) {
			std::printf("(none)");
		}
	}
	std::printf("\n");

	std::printf(
		"\tmNumMeshes=%u\n"
		"\tmNumMaterials=%u\n"
		"\tmNumAnimations=%u\n"
		"\tmNumTextures=%u\n"
		"\tmNumLights=%u\n"
		"\tmNumCameras=%u\n",
		scene->mNumMeshes,
		scene->mNumMaterials,
		scene->mNumAnimations,
		scene->mNumTextures,
		scene->mNumLights,
		scene->mNumCameras
	);

	if (scene->mMetaData) {
		std::printf("\tmMetaData=present\n");
	}
	if (scene->mName.length > 0) {
		std::printf("\tmName='%s'\n", scene->mName.C_Str());
	}
	if (scene->mNumSkeletons) {
		std::printf("\tmNumSkeletons=%u\n", scene->mNumSkeletons);
	}
}

static void print_node_recursive(const aiNode* node, unsigned int node_depth = 0)
{
	for (unsigned int i = 0; i < node_depth; ++i) {
		std::printf("\t");
	}

	std::printf("'%s': mNumMeshes=%u", node->mName.C_Str(), node->mNumMeshes);
	if (!node->mTransformation.IsIdentity()) {
		std::printf("; mTransformation=present");
	}
	std::printf("\n");
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		print_node_recursive(node->mChildren[i], node_depth + 1);
	}
}

static void print_mesh(const struct aiMesh* mesh)
{
	unsigned int numColors = 0;
	unsigned int numTextureCoords = 0;

	std::printf("'%s': ", mesh->mName.C_Str());

	std::printf("mPrimitiveTypes=");
	bool first = true;
	for (unsigned int bit = 0x1;
		bit <= _aiPrimitiveType_Force32Bit;
		bit <<= 1
	) {
		if (!(mesh->mPrimitiveTypes & bit)) {
			continue;
		}
		if (!first) {
			std::printf(",");
		}
		first = false;
		if (bit == aiPrimitiveType_POINT) {
			std::printf("POINT");
		} else if (bit == aiPrimitiveType_LINE) {
			std::printf("LINE");
		} else if (bit == aiPrimitiveType_TRIANGLE) {
			std::printf("TRIANGLE");
		} else if (bit == aiPrimitiveType_POLYGON) {
			std::printf("POLYGON");
		} else if (bit == aiPrimitiveType_NGONEncodingFlag) {
			std::printf("NGON");
		} else {
			std::printf("UNKNOWN");
		}
	}
	std::printf("; ");

	std::printf("mNumVertices=%u {%s%s%s",
		mesh->mNumVertices,
		mesh->mNormals ? "mNormals" : "",
		mesh->mTangents ? ", mTangents" : "",
		mesh->mBitangents ? ", mBitangents" : ""
	);
	for (std::size_t i = 0;
		i < sizeof(mesh->mColors) / sizeof(mesh->mColors[0]);
		++i
	) {
		if (mesh->mColors[i]) {
			++numColors;
		}
	}
	if (numColors) {
		std::printf(", mColors[%u]", numColors);
	}
	for (std::size_t i = 0;
		i < sizeof(mesh->mTextureCoords) / sizeof(mesh->mTextureCoords[0]);
		++i
	) {
		if (!mesh->mTextureCoords[i]) {
			continue;
		}
		if (numTextureCoords == 0) {
			std::printf(", mTextureCoords[");
		} else {
			std::printf(",");
		}
		const aiString* uvname = mesh->GetTextureCoordsName(i);
		if (uvname && uvname->length > 0) {
			std::printf("%s(%uD)", uvname->C_Str(), mesh->mNumUVComponents[i]);
		} else {
			std::printf("%uD", mesh->mNumUVComponents[i]);
		}
		++numTextureCoords;
	}
	if (numTextureCoords) {
		std::printf("]");
	}
	std::printf("}");

	std::printf("; mNumFaces=%u", mesh->mNumFaces);
	if (mesh->mNumBones) {
		std::printf("; mNumBones=%u", mesh->mNumBones);
	}
	std::printf("; mMaterialIndex=%u", mesh->mMaterialIndex);
	if (mesh->mNumAnimMeshes) {
		std::printf("; mNumAnimMeshes=%u", mesh->mNumAnimMeshes);
	}
	if (mesh->mMethod) {
		std::printf("; mMethod=");
		switch (mesh->mMethod) {
			case aiMorphingMethod_VERTEX_BLEND: std::printf("VERTEX_BLEND"); break;
			case aiMorphingMethod_MORPH_NORMALIZED: std::printf("MORPH_NORMALIZED"); break;
			case aiMorphingMethod_MORPH_RELATIVE: std::printf("MORPH_RELATIVE"); break;
			default: std::printf("UNKNOWN"); break;
		}
	}
	if (mesh->mAABB.mMin.x != 0.0f || mesh->mAABB.mMin.y != 0.0f || mesh->mAABB.mMin.z != 0.0f ||
		mesh->mAABB.mMax.x != 0.0f || mesh->mAABB.mMax.y != 0.0f || mesh->mAABB.mMax.z != 0.0f
	) {
		std::printf("; mAABB={ (%f,%f,%f), (%f,%f,%f) }",
			mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z,
			mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z
		);
	}

	std::printf("\n");
}

template <typename T>
static void print_material_property_array(const void* data, std::size_t length)
{
	const T* value = reinterpret_cast<const T*>(data);
	std::size_t count = length / sizeof(T);

	std::printf("{ ");
	for (std::size_t i = 0; i < count; ++i) {
		if (i) {
			std::printf(", ");
		}
		if constexpr (std::is_same_v<T, std::uint8_t>) {
			std::printf("0x%02x", static_cast<unsigned int>(value[i]));
		} else if constexpr (std::is_same_v<T, std::byte>) {
			std::printf("0x%02x", std::to_integer<unsigned int>(value[i]));
		} else if constexpr (std::is_floating_point_v<T>) {
			std::printf("%f", value[i]);
		} else {
			std::printf("%u", static_cast<unsigned int>(value[i]));
		}
	}
	std::printf(" }\n");
}

static void print_material(const struct aiMaterial* material)
{
	const aiString name = material->GetName();
	if (!name.Empty()) {
		std::printf("'%s':\n", name.C_Str());
	} else {
		std::printf("(none):\n");
	}

	// Iterate and print non-texture properties
	for (unsigned int i = 0; i < material->mNumProperties; ++i) {
		const struct aiMaterialProperty* property = material->mProperties[i];

		if (property->mSemantic != aiTextureType_NONE) {
			// Skip texture properties
			continue;
		}

		std::printf("\t%s = ", property->mKey.C_Str());

		switch (property->mType) {
			case aiPTI_Float:
				print_material_property_array<float>(property->mData, property->mDataLength);
				break;

			case aiPTI_Double:
				print_material_property_array<double>(property->mData, property->mDataLength);
				break;

			case aiPTI_String: {
				aiString s;
				material->Get(property->mKey.C_Str(), property->mSemantic, property->mIndex, s);
				std::printf("'%s'\n", s.C_Str());
				break;
			}

			case aiPTI_Integer:
				print_material_property_array<unsigned int>(property->mData, property->mDataLength);
				break;

			case aiPTI_Buffer:
				if (property->mDataLength <= 4) {
					print_material_property_array<std::byte>(property->mData, property->mDataLength);
				} else {
					std::printf("Buffer[%u]\n", property->mDataLength);
				}
				break;

			default:
				std::printf("Unknown[%u]\n", property->mDataLength);
		}
	}

	// Iterate texture properties and remember unique semantic indexes
	std::set<std::pair<unsigned int, unsigned int>> textures;
	for (unsigned int i = 0; i < material->mNumProperties; ++i) {
		const struct aiMaterialProperty* property = material->mProperties[i];

		if (property->mSemantic == aiTextureType_NONE) {
			// Skip non-texture properties
			continue;
		}

		textures.insert({ property->mSemantic, property->mIndex });
	}

	// Lookup and print texture properties
	for (const auto& texture : textures) {
		std::printf("\t%s[%u]:\n",
			aiTextureTypeToString(static_cast<aiTextureType>(texture.first)),
			texture.second
		);

		for (unsigned int i = 0; i < material->mNumProperties; ++i) {
			const struct aiMaterialProperty* property = material->mProperties[i];

			if (property->mSemantic != texture.first ||
				property->mIndex != texture.second
			) {
				continue;
			}

			std::printf("\t\t%s = ", property->mKey.C_Str());

			switch (property->mType) {
				case aiPTI_Float:
					print_material_property_array<float>(property->mData, property->mDataLength);
					break;

				case aiPTI_String: {
					aiString s;
					material->Get(property->mKey.C_Str(), property->mSemantic, property->mIndex, s);
					std::printf("'%s'\n", s.C_Str());
					break;
				}

				case aiPTI_Integer:
					print_material_property_array<unsigned int>(property->mData, property->mDataLength);
					break;

				case aiPTI_Buffer:
					if (property->mDataLength <= 4) {
						print_material_property_array<std::byte>(property->mData, property->mDataLength);
					} else {
						std::printf("Buffer[%u]\n", property->mDataLength);
					}
					break;

				default:
					std::printf("Unknown[%u]\n", property->mDataLength);
			}
		}
	}
}

static void print_texture(unsigned int idx, const struct aiTexture* texture)
{
	if (texture->mHeight) {
		// uncompressed texture
		std::printf("\t[%u]: %ux%u, ARGB8888\n", idx, texture->mWidth, texture->mHeight);
	} else {
		// compressed texture
		std::printf("\t[%u]: %u bytes, %s\n", idx, texture->mWidth, texture->achFormatHint);
	}
}

static void print_light(const aiLight* light)
{
	const char* type;

	switch (light->mType) {
		case aiLightSource_UNDEFINED: type = "UNDEFINED"; break;
		case aiLightSource_DIRECTIONAL: type = "DIRECTIONAL"; break;
		case aiLightSource_POINT: type = "POINT"; break;
		case aiLightSource_SPOT: type = "SPOT"; break;
		case aiLightSource_AMBIENT: type = "AMBIENT"; break;
		case aiLightSource_AREA: type = "AREA"; break;
		default: type = "UNKNOWN"; break;
	}

	std::printf("'%s': mType=%s\n", light->mName.C_Str(), type);
	if (light->mType != aiLightSource_DIRECTIONAL) {
		print_vector("mPosition", light->mPosition);
	}
	if (light->mType != aiLightSource_POINT) {
		print_vector("mDirection", light->mDirection);
		print_vector("mUp", light->mUp);
	}
	if (light->mType != aiLightSource_DIRECTIONAL) {
		std::printf("\tmAttenuation = { constant=%f, linear=%f, quadratic=%f }\n",
			light->mAttenuationConstant,
			light->mAttenuationLinear,
			light->mAttenuationQuadratic
		);
	}
	print_vector("mColorDiffuse", light->mColorDiffuse);
	print_vector("mColorSpecular", light->mColorSpecular);
	print_vector("mColorAmbient", light->mColorAmbient);
	if (light->mType == aiLightSource_SPOT) {
		std::printf("\tmAngle = { inner=%f, outer=%f }\n",
			light->mAngleInnerCone,
			light->mAngleOuterCone
		);
	}
	if (light->mType == aiLightSource_AREA) {
		print_vector("mSize", light->mSize);
	}
}

static void print_camera(const aiCamera* camera)
{
	std::printf("'%s':\n", camera->mName.C_Str());
	print_vector("mPosition", camera->mPosition);
	print_vector("mUp", camera->mUp);
	print_vector("mLookAt", camera->mLookAt);
	if (camera->mHorizontalFOV != 0.0f) {
		std::printf("\tmHorizontalFOV = %f\n", camera->mHorizontalFOV);
	}
	std::printf("\tmClipPlane = { near=%f, far=%f }\n",
		camera->mClipPlaneNear,
		camera->mClipPlaneFar
	);
	if (camera->mAspect != 0.0f) {
		std::printf("\tmAspect = %f\n", camera->mAspect);
	}
	if (camera->mOrthographicWidth != 0.0f) {
		std::printf("\tmOrthographicWidth = %f\n", camera->mOrthographicWidth);
	}
}

static int import_scene(const char* filename)
{
	const std::string display_name = std::filesystem::path(filename).filename().string();
	Assimp::Importer importer;
	const aiScene* scene;

	scene = importer.ReadFile(filename,
		aiProcessPreset_TargetRealtime_MaxQuality
// 		aiProcess_JoinIdenticalVertices |
// 		aiProcess_Triangulate |
// 		aiProcess_ValidateDataStructure |
// 		aiProcess_RemoveRedundantMaterials |
// 		aiProcess_SortByPType
	);
	if (!scene) {
		Assimp::DefaultLogger::get()->error(importer.GetErrorString());
		return -1;
	}

	print_summary(display_name.c_str(), scene);

	std::printf("\n%s nodes:\n", display_name.c_str());
	print_node_recursive(scene->mRootNode);

	if (scene->mNumMeshes) {
		std::printf("\n%s meshes[%u]:\n", display_name.c_str(), scene->mNumMeshes);
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			print_mesh(scene->mMeshes[i]);
		}
	}

	if (scene->mNumMaterials) {
		std::printf("\n%s materials[%u]:\n", display_name.c_str(), scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
			print_material(scene->mMaterials[i]);
		}
	}

	if (scene->mNumTextures) {
		std::printf("\n%s textures[%u]:\n", display_name.c_str(), scene->mNumTextures);
		for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
			print_texture(i, scene->mTextures[i]);
		}
	}

	if (scene->mNumLights) {
		std::printf("\n%s lights[%u]:\n", display_name.c_str(), scene->mNumLights);
		for (unsigned int i = 0; i < scene->mNumLights; ++i) {
			print_light(scene->mLights[i]);
		}
	}

	if (scene->mNumCameras) {
		std::printf("\n%s cameras[%u]:\n", display_name.c_str(), scene->mNumCameras);
		for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
			print_camera(scene->mCameras[i]);
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	int r;

	if (argc != 2) {
		std::fprintf(stderr, "No import file specified\n");
		return 1;
	}

	Assimp::DefaultLogger::create(nullptr, Assimp::Logger::VERBOSE, aiDefaultLogStream_STDERR);

	r = import_scene(argv[1]);
	if (r) {
		std::fprintf(stderr, "Failed to import scene from '%s'\n", argv[1]);
	}

	Assimp::DefaultLogger::kill();
	return r;
}
