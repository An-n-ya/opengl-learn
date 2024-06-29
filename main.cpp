#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "utils.h"
#include "Sphere.h"
#include "Torus.h"

#define numVAOs 1
#define numVBOs 4
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

GLint vLoc, projLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat;

GLuint brickTexture;

Sphere mySphere(48);
Torus myTorus;

void setup_vertices() {
    auto ind = myTorus.getIndices();
    auto vert = myTorus.getVertices();
    auto text = myTorus.getTextCoords();
    auto norm = myTorus.getNormals();

    std::vector<float> flat_vert;
    std::vector<float> flat_text;
    std::vector<float> flat_norm;

    int numIndices = myTorus.getNumIndices();
    for (int i = 0; i < numIndices; i++) {
        flat_vert.push_back((vert[i]).x);
        flat_vert.push_back((vert[i]).y);
        flat_vert.push_back((vert[i]).z);

        flat_text.push_back((text[i]).s);
        flat_text.push_back((text[i]).t);

        flat_norm.push_back((norm[i]).x);
        flat_norm.push_back((norm[i]).y);
        flat_norm.push_back((norm[i]).z);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, flat_vert.size() * 4, &flat_vert[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, flat_text.size() * 4, &flat_text[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, flat_norm.size() * 4, &flat_norm[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}


void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
    brickTexture = loadTexture("../assets/LunaTextures+NormalMaps/brick1.jpg");
//    glGenVertexArrays(numVAOs, vao);
//    glBindVertexArray(vao[0]);
    setup_vertices();
}

float x = 0;
float inc = 1.0f;
void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);
    glClearColor(0.2, 0.5,0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    tfLoc = glGetUniformLocation(renderingProgram, "tf");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float) width / (float) height;
    // 1.0472 is 60 degree
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

//    if (x > 100.0) inc = -1.0f;
//    if (x < -100.0) inc = 1.0f;
//    x += inc;

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-(cameraX + x), -(cameraY + x), -cameraZ));

    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniform1f(tfLoc, (float)currentTime);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    int vert_num = myTorus.getNumIndices();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, vert_num, GL_UNSIGNED_INT, nullptr);
//    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
//    glDrawArrays(GL_TRIANGLES, 0, 18);
//    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

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
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
