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
#include "ObjImporter.h"

#define numVAOs 1
#define numVBOs 4
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

GLint vLoc, projLoc, tfLoc, nLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;

GLint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
glm::vec3 currentLightPos, lightPosV;
float lightPos[3];

glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);

GLuint brickTexture;

Sphere mySphere(48);
Torus myTorus;
Model myModel("../assets/NasaShuttle/shuttle.obj");

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// gold material
float* matAmb = Material::goldAmbient();
float* matDif = Material::goldDiffuse();
float* matSpe = Material::goldSpecular();
float matShi = Material::goldShininess();

void installLights(glm::mat4 vMatrix) {
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");

    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);

    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, matShi);
    checkOpenGLError();
}


void setup_vertices() {
    auto vert = myModel.getVertices();
    auto text = myModel.getTextCoords();
    auto norm = myModel.getNormals();

    std::vector<float> flat_vert;
    std::vector<float> flat_text;
    std::vector<float> flat_norm;

    int numIndices = myModel.getNumVertices();
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
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}


void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
//    brickTexture = loadTexture("../assets/NasaShuttle/spstob_1.jpg");
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

    vLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
    tfLoc = glGetUniformLocation(renderingProgram, "tf");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float) width / (float) height;
    // 1.0472 is 60 degree
    pMat = glm::perspective(0.5f, aspect, 0.1f, 1000.0f);

//    if (x > 100.0) inc = -1.0f;
//    if (x < -100.0) inc = 1.0f;
//    x += inc;

    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
    mMat *= glm::rotate(mMat, 20.0f / 360.0f * 2.0f * 3.1415f, glm::vec3(1.0f, 0.0f, 0.0f));
    mMat *= glm::rotate(mMat, 20.0f / 360.0f * 2.0f * 3.1415f, glm::vec3(0.0f, 1.0f, 0.0f));
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-(cameraX + x), -(cameraY + x), -cameraZ));

    mvMat = vMat * mMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));


    currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
    installLights(vMat);


    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniform1f(tfLoc, (float)currentTime);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, brickTexture);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
//    int vert_num = myTorus.getNumIndices();
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
//    glDrawElements(GL_TRIANGLES, vert_num, GL_UNSIGNED_INT, nullptr);
//    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
//    glDrawArrays(GL_TRIANGLES, 0, 18);
    glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

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
