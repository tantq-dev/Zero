#pragma once

#include "core/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

namespace Core {

	class ModelLoader {
	public:
		ModelLoader() = default;
		~ModelLoader() = default;

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes) {
			// Process all the node's meshes
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

				// Get node's transformation matrix
				aiMatrix4x4 transform = node->mTransformation;

				// Extract scale from the transformation matrix
				aiVector3D scale;
				aiQuaternion rotation;
				aiVector3D position;
				transform.Decompose(scale, rotation, position);

				meshes.push_back(ProcessMesh(scene->mMeshes[i], scene, scale));

			}

			// Process all the node's children
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				ProcessNode(node->mChildren[i], scene, meshes);
			}
		}

		// Load a model, returning a shared_ptr to a single combined mesh
		std::shared_ptr<Mesh> LoadModel(const std::string& filename) {
			// Create an instance of the Assimp importer
			Assimp::Importer importer;

			importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 1);

			unsigned int flags =
				aiProcess_Triangulate |        // Triangulate polygons
				aiProcess_JoinIdenticalVertices |  // Join identical vertices
				aiProcess_SortByPType |        // Split meshes by primitive type
				aiProcess_GenNormals |         // Generate normals if not present
				aiProcess_GlobalScale |        // Apply global scale factor
				aiProcess_LimitBoneWeights;

			// Read the file with post-processing
			const aiScene* scene = importer.ReadFile(filename, flags);



			// Check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
				return nullptr;
			}

			glm::vec3 scale;

			std::vector<Mesh> meshes;
			ProcessNode(scene->mRootNode, scene, meshes);
			// Process all meshes in the file


			LOG_INFO("Number of mesh: " + std::to_string(scene->mNumMeshes));


			// Combine all meshes into one
			return CombineMeshes(meshes, filename);
		}

	private:
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, aiVector3D scale) {
			Mesh result;
			result.name = mesh->mName.C_Str();
			if (result.name.empty()) {
				result.name = "unnamed_mesh";
			}

			// Process vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;

				// Position
				vertex.position.x = mesh->mVertices[i].x * scale.x;
				vertex.position.y = mesh->mVertices[i].y * scale.y;
				vertex.position.z = mesh->mVertices[i].z * scale.z;

				// Normal
				if (mesh->HasNormals()) {
					vertex.normal.x = mesh->mNormals[i].x;
					vertex.normal.y = mesh->mNormals[i].y;
					vertex.normal.z = mesh->mNormals[i].z;
				}
				else {
					vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal
				}

				// Texture coordinates
				//!Dont need now 
				//if (mesh->mTextureCoords[0]) {
				//	vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
				//	vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
				//}
				//else {
				//	vertex.texCoord = glm::vec2(0.0f, 0.0f); // Default UV
				//}

				// Set default color
				vertex.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

				// Set vertex color from material if available
				// Dont need now
			/*	if (mesh->mMaterialIndex >= 0) {
					aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
					aiColor4D diffuse;
					if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
						vertex.color.r = diffuse.r;
						vertex.color.g = diffuse.g;
						vertex.color.b = diffuse.b;
						vertex.color.a = diffuse.a;
					}
				}*/

				result.vertices.push_back(vertex);
			}

			// Process indices
			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					result.indices.push_back(face.mIndices[j]);
				}
			}

			return result;
		}

		// Combine multiple meshes into a single mesh
		std::shared_ptr<Mesh> CombineMeshes(const std::vector<Mesh>& meshes, const std::string& filename) {
			if (meshes.empty()) {
				return nullptr;
			}

			// If there's only one mesh, return it directly
			if (meshes.size() == 1) {
				auto result = std::make_shared<Mesh>(meshes[0]);
				// Extract file name from path for the mesh name
				size_t lastSlash = filename.find_last_of("/\\");
				size_t lastDot = filename.find_last_of(".");
				std::string baseName = (lastSlash == std::string::npos) ?
					filename.substr(0, lastDot) :
					filename.substr(lastSlash + 1, lastDot - lastSlash - 1);
				result->name = baseName;
				return result;
			}

			// Create a new mesh to hold all the combined data
			auto combinedMesh = std::make_shared<Mesh>();

			// Extract file name from path for the mesh name
			size_t lastSlash = filename.find_last_of("/\\");
			size_t lastDot = filename.find_last_of(".");
			std::string baseName = (lastSlash == std::string::npos) ?
				filename.substr(0, lastDot) :
				filename.substr(lastSlash + 1, lastDot - lastSlash - 1);
			combinedMesh->name = baseName + "_combined";

			// Combine all meshes
			uint32_t indexOffset = 0;
			for (const auto& mesh : meshes) {
				// Add vertices
				combinedMesh->vertices.insert(
					combinedMesh->vertices.end(),
					mesh.vertices.begin(),
					mesh.vertices.end()
				);

				// Add indices with offset
				for (uint32_t index : mesh.indices) {
					combinedMesh->indices.push_back(index + indexOffset);
				}

				// Update index offset for the next mesh
				indexOffset += static_cast<uint32_t>(mesh.vertices.size());
			}

			return combinedMesh;
		}
	};



} // namespace Zero