//
// Created by annya on 6/27/24.
//

#ifndef OPENGL_LEARN_UTILS_H
#define OPENGL_LEARN_UTILS_H
#include <iostream>
#include <string>
#include <fstream>

#endif //OPENGL_LEARN_UTILS_H

void printShaderLog(GLuint shader) {
    int len = 0;
    int chWritten = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &chWritten, log);
        std::cout << "Shader Info Log: " << log << std::endl;
        free(log);
    }
}
void printProgramLog(GLuint prog) {
    int len = 0;
    int chWritten = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        glGetProgramInfoLog(prog, len, &chWritten, log);
        std::cout << "Program Info Log: " << log << std::endl;
        free(log);
    }
}
bool checkOpenGLError() {
    bool foundError = false;
    unsigned int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        std::cout << "glError: "  << glErr << std::endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

std::string readShaderSource(const char *filepath) {
    std::string content;
    std::ifstream fileStream(filepath, std::ios::in);
    std::string line;
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n") ;
    }
    fileStream.close();
    return content;
}

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


