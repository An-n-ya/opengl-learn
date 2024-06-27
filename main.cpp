#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils.h"

#define numVAOs 1
GLuint renderingProgram;
GLuint vao[numVAOs];

GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;
    std::string vertShaderStr = readShaderSource("../glsl/vertShader.glsl");
    std::string fragShaderStr = readShaderSource("../glsl/fragShader.glsl");

    const char *vshaderSource = vertShaderStr.c_str();
    const char *fshaderSource = fragShaderStr.c_str();

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vshader, 1, &vshaderSource, NULL);
    glShaderSource(fshader, 1, &fshaderSource, NULL);
    glCompileShader(vshader);
    checkOpenGLError();
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        std::cout << "vertex compilation failed" << std::endl;
        printShaderLog(vshader);
    }

    glCompileShader(fshader);
    checkOpenGLError();
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        std::cout << "fragment compilation failed" << std::endl;
        printShaderLog(fshader);
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vshader);
    glAttachShader(vfProgram, fshader);
    glLinkProgram(vfProgram);
    checkOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        std::cout << "linking failed" << std::endl;
        printProgramLog(vfProgram);
    }

    return vfProgram;
}

void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

float x = 0;
float inc = 0.01f;


void display(GLFWwindow* window) {
    glUseProgram(renderingProgram);
    glClearColor(0.2, 0.5,0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    x += inc;
    if (x > 0.75f) inc = -0.01f;
    if (x < -0.75f) inc = 0.01f;
    GLint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
    glProgramUniform1f(renderingProgram, offsetLoc, x);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "opengl-learn", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    init(window);

    while(!glfwWindowShouldClose(window)) {
        display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
