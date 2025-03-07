#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <utility>

GLuint shaderProgram;

// Callback function to adjust the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
        (void)window;
        glViewport(0, 0, width, height);
}  

// Vertex Shader and Fragment Shader (same as before)
const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 trans;
out vec3 vertexColor;
void main()
{
    gl_Position = trans * vec4(aPos, 1.0f);
    vertexColor = aColor;
})";

const char* fragmentShaderSource = R"(
#version 460 core
in vec3 vertexColor;
out vec4 FragColor;
void main()
{
    FragColor = vec4(vertexColor, 1.0f);
})";

// Pyramid class definition
class Pyramid {
private:
    GLuint VBO, VAO, EBO;
    glm::mat4 rotationMatrixX, rotationMatrixY, rotationMatrixZ, translationMatrix, scalingMatrix;
    GLuint uniTrans;

    static constexpr const std::array<glm::vec3, 12> vertices = {{
        {0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f},         // top-center           0
        {0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f},        // back-right           1
        {-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f},       // back-left            2
        {0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f},       // front-right          3
        {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f},      // front-left           4
        {0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}        	// bottom-center        5
    }};

    static constexpr const std::array<unsigned int, 24> indices = {{
        0,1,2, // face 1
        0,3,4, // face 2
        0,1,3, // face 3
        0,2,4, // face 4

	// Bottom
        5,1,2,
        5,1,3,
        5,3,4,
        5,2,4
    }};

public:

    Pyramid(GLuint shaderProgram)
        : VBO{},
          VAO{},
          EBO{},
          rotationMatrixX{1.0f},
          rotationMatrixY{1.0f},
          rotationMatrixZ{1.0f},
          translationMatrix{1.0f},
          scalingMatrix{1.0f},
          uniTrans{}
    {
        // Initialize OpenGL resources
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glUseProgram(shaderProgram);
        uniTrans = glGetUniformLocation(shaderProgram, "trans");
    }


    template <typename vec3 = glm::vec3>
    void translate(vec3&& translation) {
	translationMatrix = glm::translate(
	    translationMatrix,
	    std::forward<vec3>(translation)
	);
    }

    template <typename vec3 = glm::vec3>
    void scale(vec3&& translation) {
	scalingMatrix = glm::scale(
	    glm::mat4(1.0f),
	    std::forward<vec3>(translation)
	);
    }

    void rotateX(float angleDegrees) {
	rotationMatrixX = glm::rotate(
	    rotationMatrixX,
	    glm::radians(angleDegrees),
	    glm::vec3(1.0f, 0.0f, 0.0f)
	);
    }

    void rotateY(float angleDegrees) {
	rotationMatrixY = glm::rotate(
	    rotationMatrixY,
	    glm::radians(angleDegrees),
	    glm::vec3(0.0f, 1.0f, 0.0f)
	);
    }

    void rotateZ(float angleDegrees) {
	rotationMatrixZ = glm::rotate(
	    rotationMatrixZ,
	    glm::radians(angleDegrees),
	    glm::vec3(0.0f, 0.0f, 1.0f)
	);
    }

    void draw() {
	glBindVertexArray(VAO);

        // Combine the rotations and send the transformation matrix to the shader
        glm::mat4 final_transformation =
		translationMatrix
		* rotationMatrixX
		* rotationMatrixY
		* rotationMatrixZ
		* scalingMatrix;
	
        glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(final_transformation));

        // Draw the pyramid
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
    }

    void cleanup() {
        // Cleanup resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    ~Pyramid() {}
};

// GLFW window and OpenGL setup (same as before)
void init_gl_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize and configure GLFW
    init_gl_window();

    // Create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Multi-Colored Pyramids", NULL, NULL);
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
    glEnable(GL_BLEND); // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int success{0};
    char infoLog[512] = {0};

    // Compile shaders and create shader program 
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	return -1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	return -1;
    }

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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<Pyramid> pyramids;
    int num_rows = 10;
    int num_cols = num_rows;
    float vertical_offset = (2.0f / (float)num_rows);
    float horizontal_offset = (2.0f / (float)num_rows);

    for (int i = 0; i < num_rows; ++i) {
	for (int j = 0; j < num_cols; ++j) {

	    Pyramid pyramid(shaderProgram);

	    pyramid.translate(
		glm::vec3(
		    (-2.0f + horizontal_offset * (1 + i + (j<<1)) ) / 2.0f,
		    (-2.0f + vertical_offset   * (1 + 0 + (i<<1)) ) / 2.0f,
		    0.0f
		)
	    );

	    pyramid.scale(
		glm::vec3(
		    1.0f / (float)(num_rows + num_rows),
		    1.0f / (float)(num_rows + num_rows),
		    1.0f / (float)(num_rows + num_rows)
		)
	    );

	    if ((j + i) % 2 == 0) {
		pyramid.rotateX(180.0f);
	    }

	    pyramids.emplace_back(pyramid);
	}

	num_cols--;
    }

    // Main render loop
    auto t_start = std::chrono::high_resolution_clock::now();
    float last_time = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update rotation
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        float delta_time = time - last_time;
        last_time = time;

	for (auto& pyramid : pyramids) {
	    pyramid.rotateY(720.0f * delta_time);
	    pyramid.rotateX(180.0f * delta_time);
	    pyramid.draw();
	}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto& pyramid : pyramids) {
	pyramid.cleanup();
    }

    // Cleanup and terminate
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
