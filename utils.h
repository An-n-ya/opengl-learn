//
// Created by annya on 6/27/24.
//

#ifndef OPENGL_LEARN_UTILS_H
#define OPENGL_LEARN_UTILS_H
#include <iostream>
#include <string>
#include <fstream>
#include "SOIL2.h"

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

GLuint createShaderProgram(const char* vertFilePath, const char* fragFilePath) {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;
//    std::string vertShaderStr = readShaderSource("../glsl/gouraud/vertShaderglsl");
//    std::string fragShaderStr = readShaderSource("../glsl/gouraud/fragShader.glsl");
//    std::string vertShaderStr = readShaderSource("../glsl/phong/vertShader1.glsl");
//    std::string fragShaderStr = readShaderSource("../glsl/phong/fragShader1.glsl");
    std::string vertShaderStr = readShaderSource(vertFilePath);
    std::string fragShaderStr = readShaderSource(fragFilePath);

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

GLuint loadTexture(const char *textImagePath) {
    GLuint textureID;
    textureID = SOIL_load_OGL_texture(textImagePath,
                                      SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureID == 0) std::cout << "could not find texture file " << textImagePath << std::endl;
    return textureID;
}

namespace Material {
    // GOLD material - ambient, diffuse, specular, and shininess
    float* normalAmbient() { static float a[4] = { 1.0f, 1.0f, 1.0f, 1 }; return (float*)a; }
    float* normalDiffuse() { static float a[4] = { 1.0f, 1.0f, 1.0f, 1 }; return (float*)a; }
    float* normalSpecular() { static float a[4] = { 1.0f, 1.0f, 1.0f, 1 }; return (float*)a; }
    float normalShininess() { return 11.2f; }

    // GOLD material - ambient, diffuse, specular, and shininess
    float* goldAmbient() { static float a[4] = { 0.2473f, 0.1995f, 0.0745f, 1 }; return (float*)a; }
    float* goldDiffuse() { static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 }; return (float*)a; }
    float* goldSpecular() { static float a[4] = { 0.6283f, 0.5559f, 0.3661f, 1 }; return (float*)a; }
    float goldShininess() { return 51.2f; }

// SILVER material - ambient, diffuse, specular, and shininess
    float* silverAmbient() { static float a[4] = { 0.1923f, 0.1923f, 0.1923f, 1 }; return (float*)a; }
    float* silverDiffuse() { static float a[4] = { 0.5075f, 0.5075f, 0.5075f, 1 }; return (float*)a; }
    float* silverSpecular() { static float a[4] = { 0.5083f, 0.5083f, 0.5083f, 1 }; return (float*)a; }
    float silverShininess() { return 51.2f; }

// BRONZE material - ambient, diffuse, specular, and shininess
    float* bronzeAmbient() { static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 }; return (float*)a; }
    float* bronzeDiffuse() { static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 }; return (float*)a; }
    float* bronzeSpecular() { static float a[4] = { 0.3936f, 0.2719f, 0.1667f, 1 }; return (float*)a; }
    float bronzeShininess() { return 25.6f; }

}

