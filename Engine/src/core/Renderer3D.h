#pragma once

// Include GLAD before other OpenGL-related headers
#include <glad/glad.h>
#include "Mesh.h"
#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Core {

	struct Camera3D {
		glm::vec3 position = glm::vec3(0.0f, 0.0f, -5.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); // Pitch, yaw, roll
		float fov = 60.0f;
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;

		glm::mat4 GetViewMatrix() {
			glm::mat4 view = glm::mat4(1.0f);

			// Apply camera rotation (order: yaw, pitch, roll)
			view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			// Apply camera translation
			view = glm::translate(view, -position);

			return view;
		}

		glm::mat4 GetProjectionMatrix(float aspectRatio) {
			return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
		}
	};

	struct Transform {
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::mat4 GetModelMatrix() const {
			glm::mat4 model = glm::mat4(1.0f);

			// Apply transformations (order: scale, rotate, translate)
			model = glm::translate(model, position);

			// Apply rotation (order: yaw, pitch, roll)
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			model = glm::scale(model, scale);

			return model;
		}
	};

	// Simple shader class
	class Shader {
	public:
		Shader() : programID(0) {}

		bool Load(const std::string& vertexCode, const std::string& fragmentCode) {
			// Create shader program
			programID = glCreateProgram();

			// Compile vertex shader
			GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
			if (vertexShader == 0) return false;

			// Compile fragment shader
			GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);
			if (fragmentShader == 0) {
				glDeleteShader(vertexShader);
				return false;
			}

			// Link program
			glAttachShader(programID, vertexShader);
			glAttachShader(programID, fragmentShader);
			glLinkProgram(programID);

			// Check linking status
			GLint success;
			glGetProgramiv(programID, GL_LINK_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetProgramInfoLog(programID, 512, NULL, infoLog);
				SDL_Log("Shader program linking failed: %s", infoLog);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				glDeleteProgram(programID);
				programID = 0;
				return false;
			}

			// Clean up shaders as they're linked to the program now
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return true;
		}

		void Use() {
			glUseProgram(programID);
		}

		void SetBool(const std::string& name, bool value) const {
			glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
		}

		void SetInt(const std::string& name, int value) const {
			glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
		}

		void SetFloat(const std::string& name, float value) const {
			glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
		}

		void SetVec3(const std::string& name, const glm::vec3& value) const {
			glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value));
		}

		void SetMat4(const std::string& name, const glm::mat4& mat) const {
			glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		~Shader() {
			if (programID != 0) {
				glDeleteProgram(programID);
			}
		}

	private:
		GLuint programID;

		GLuint CompileShader(GLenum type, const std::string& source) {
			GLuint shader = glCreateShader(type);
			const char* src = source.c_str();
			glShaderSource(shader, 1, &src, NULL);
			glCompileShader(shader);

			// Check compilation status
			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				SDL_Log("Shader compilation failed: %s", infoLog);
				glDeleteShader(shader);
				return 0;
			}

			return shader;
		}
	};

	// OpenGL Mesh representation
	class GLMesh {
	public:
		GLMesh() : VAO(0), VBO(0), EBO(0), indexCount(0) {}

		void Load(const Mesh& mesh) {
			// Generate buffers and VAO
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			// Bind VAO
			glBindVertexArray(VAO);

			// Bind and fill vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

			// Bind and fill index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

			// Set up vertex attribute pointers

			// Position attribute
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

			// Normal attribute
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			// Texture coordinate attribute
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

			// Color attribute
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

			// Unbind VAO to prevent further modifications
			glBindVertexArray(0);

			// Store index count for rendering
			indexCount = static_cast<GLsizei>(mesh.indices.size());
		}

		void Draw() {
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		~GLMesh() {
			if (VAO != 0) {
				glDeleteVertexArrays(1, &VAO);
				glDeleteBuffers(1, &VBO);
				glDeleteBuffers(1, &EBO);
			}
		}

	private:
		GLuint VAO, VBO, EBO;
		GLsizei indexCount;
	};

	class Renderer3D {
	public:
		Renderer3D(SDL_Window* window, int width, int height)
			: window(window), width(width), height(height) {

			// Create OpenGL context
			glContext = SDL_GL_CreateContext(window);
			if (!glContext) {
				SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
				return;
			}

			// Initialize GLAD
			if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
				SDL_Log("Failed to initialize GLAD");
				return;
			}

			// Set up OpenGL state
			glViewport(0, 0, width, height);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			// Load default shader
			const char* vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec2 aTexCoord;
            layout (location = 3) in vec4 aColor;
            
            out vec3 FragPos;
            out vec3 Normal;
            out vec2 TexCoord;
            out vec4 Color;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            void main() {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                TexCoord = aTexCoord;
                Color = aColor;
                
                gl_Position = projection * view * model * vec4(aPos, 1.0);
            }
        )";

			const char* fragmentShaderSource = R"(
            #version 330 core
            in vec3 FragPos;
            in vec3 Normal;
            in vec2 TexCoord;
            in vec4 Color;
            
            out vec4 FragColor;
            
            uniform vec3 lightDir;
            uniform vec3 lightColor;
            uniform float ambientStrength;
            
            void main() {
                // Ambient
                vec3 ambient = ambientStrength * lightColor;
                
                // Diffuse
                vec3 norm = normalize(Normal);
                float diff = max(dot(norm, -normalize(lightDir)), 0.0);
                vec3 diffuse = diff * lightColor;
                
                // Final color
                vec3 result = (ambient + diffuse) * Color.rgb;
                FragColor = vec4(result, Color.a);
            }
        )";

			// Compile and link shader program
			defaultShader.Load(vertexShaderSource, fragmentShaderSource);

			// Set default light parameters
			defaultShader.Use();
			defaultShader.SetVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.0f));
			defaultShader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
			defaultShader.SetFloat("ambientStrength", 0.2f);
		}

		~Renderer3D() {
			// Clean up resources
			meshCache.clear();
		}

		void SetCamera(const Camera3D& cam) {
			camera = cam;
		}

		void Clear() {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void RenderMesh(const Mesh& mesh, const Transform& transform) {
			// Get or create a GLMesh for this mesh
			GLMesh* glMesh = GetGLMesh(mesh);
			if (!glMesh) return;

			// Use shader
			defaultShader.Use();

			// Set matrices
			float aspectRatio = static_cast<float>(width) / height;
			glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model = transform.GetModelMatrix();

			defaultShader.SetMat4("projection", projection);
			defaultShader.SetMat4("view", view);
			defaultShader.SetMat4("model", model);

			// Draw the mesh
			glMesh->Draw();
		}

		void Present() {
			SDL_GL_SwapWindow(window);
		}

	private:
		GLMesh* GetGLMesh(const Mesh& mesh) {
			// Use mesh name as a cache key, or a generic id if name is empty
			std::string key = mesh.name.empty() ? "anonymous_mesh_" + std::to_string(meshCache.size()) : mesh.name;

			// If mesh is not in cache, create and store it
			if (meshCache.find(key) == meshCache.end()) {
				meshCache[key] = std::make_unique<GLMesh>();
				meshCache[key]->Load(mesh);
			}

			return meshCache[key].get();
		}

	private:
		SDL_Window* window;
		SDL_GLContext glContext;
		int width, height;
		Camera3D camera;
		Shader defaultShader;
		std::unordered_map<std::string, std::unique_ptr<GLMesh>> meshCache;
	};

} // namespace Zero