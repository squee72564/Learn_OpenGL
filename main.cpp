#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <chrono>

GLuint VBO[1];
GLuint VAO[1];
GLuint EBO[1];
GLint uniTrans;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

// Vertex Shader: Receives position and color from the application and passes them to the fragment shader
const char *vertexShaderSource = "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n" 	// Position of the vertex
        "layout (location = 1) in vec3 aColor;\n" 	// Color of the vertex
	"uniform mat4 trans;\n"				// Uniform for transformation matrix
        "out vec3 vertexColor;\n" 			// Output the color to the fragment shader
        "void main()\n"
        "{\n"
        "       gl_Position = trans * vec4(aPos, 1.0f);\n"		// Set position & apply transformation
        "       vertexColor = aColor;\n" 				// Pass color to fragment shader
        "}\0";

// Fragment Shader: Receives the interpolated color and applies it to each fragment
const char *fragmentShaderSource = "#version 460 core\n"
        "in vec3 vertexColor;\n"			// Interpolated color from vertex shader
        "out vec4 FragColor;\n"				// Output color
        "void main()\n"
        "{\n"
        "       FragColor = vec4(vertexColor, 1.0f);\n" // Use interpolated color for the fragment
        "}\0";

// Callback function to adjust the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
        (void)window;
        glViewport(0, 0, width, height);
}  

// Initialize GLFW for OpenGL window creation
void init_gl_window(void) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Process user input (e.g., ESC key to close the window)
void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
}

int main() {
        // Initialize and configure GLFW
        init_gl_window();

        // Create window
        GLFWwindow *window = glfwCreateWindow(800, 600, "Multi-Colored Pyramid", NULL, NULL); 
        if (window == nullptr) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return -1;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Load OpenGL function pointers with GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cerr << "Failed to init GLAD" << std::endl;
                glfwTerminate();
                return -1;
        }

	glEnable(GL_DEPTH_TEST); // Enable depth testing

        // Compile shaders
        int success{0};
        char infoLog[512] = {0};

        // Vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
                return -1;
        }

        // Fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                return -1;
        }

        // Shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if(!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
                return -1;
        }

        // Clean up shaders as they are already linked
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Define pyramid vertices and colors
        std::vector<glm::vec3> vertices{
                {0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, 	// top-center		0
                {0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f},	// back-right		1
                {-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f},	// back-left		2
                {0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f},	// front-right		3
                {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f},	// front-left		4
                {0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f},	// bottom-center	5
        };

	std::vector<unsigned int> indices{
		0,1,2, // face 1
		0,3,4, // face 2
		0,1,3, // face 3
		0,2,4, // face 4
		
		5,1,2,
		5,1,3,
		5,3,4,
		5,2,4
	};
	
        // Set up buffers
        glGenVertexArrays(1, VAO);
        glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

        glBindVertexArray(VAO[0]);

        // Generate and bind vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

	glUseProgram(shaderProgram);

	uniTrans = glGetUniformLocation(shaderProgram, "trans");

	auto t_start = std::chrono::high_resolution_clock::now();
	float last_time = 0.0f;

	glm::mat4 rotationMatrixX = glm::mat4(1.0f);
	glm::mat4 rotationMatrixY = glm::mat4(1.0f);

        // Main render loop
        while(!glfwWindowShouldClose(window)) {
                processInput(window);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

		// Calculate transformation
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		float delta_time = time - last_time;
		last_time = time;

		rotationMatrixY = glm::rotate(rotationMatrixY, glm::radians(360.0f * delta_time), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
		rotationMatrixX = glm::rotate(rotationMatrixX, glm::radians(30.0f * delta_time), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis

		glm::mat4 combinedRotation = rotationMatrixX * rotationMatrixY; 

		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(combinedRotation));

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        // Clean up resources
        glDeleteVertexArrays(1, VAO);
        glDeleteBuffers(1, VBO);
        glDeleteProgram(shaderProgram);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
}

