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
#define numVBOs 7
GLuint renderingProgram1, renderingProgram2, renderingProgram3, currentProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

GLint vLoc, projLoc, tfLoc, nLoc, sLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, sMat, invTrMat;

GLint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
glm::vec3 currentLightPos, lightPosV;
float lightPos[3];

glm::vec3 initialLightLoc = glm::vec3(3.8f, 2.2f, 1.1f);

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

GLuint brickTexture;

Sphere mySphere(48);
Torus myTorus;
//Model myModel("../assets/Studio522Dolphin/dolphinHighPoly.obj");
Model myModel("../assets/otherModels/pyr.obj");

glm::vec3 torusLoc = glm::vec3(-1.6f, 0.0f, -0.3f);
glm::vec3 pyrLoc = glm::vec3(1.0f, 0.1f, 0.3f);

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// material
float* matAmb = Material::normalAmbient();
float* matDif = Material::normalDiffuse();
float* matSpe = Material::normalSpecular();
float matShi = Material::normalShininess();

// shadow
int screenSizeX, screenSizeY;
GLuint  shadowTex, shadowBuffer;
glm::mat4 lightVMatrix, lightPMatrix;
glm::mat4 shadowMVP1, shadowMVP2;
glm::mat4 b = glm::mat4(
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f
);

void setupShadowBuffers(GLFWwindow *window) {
    glfwGetFramebufferSize(window, &width, &height) ;
    screenSizeX = width;
    screenSizeY = height;

    glGenFramebuffers(1, &shadowBuffer);

    glGenTextures(1,  &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void installLights(GLuint program, glm::mat4 vMatrix) {
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    globalAmbLoc = glGetUniformLocation(program, "globalAmbient");
    ambLoc = glGetUniformLocation(program, "light.ambient");
    diffLoc = glGetUniformLocation(program, "light.diffuse");
    specLoc = glGetUniformLocation(program, "light.specular");
    posLoc = glGetUniformLocation(program, "light.position");

    mAmbLoc = glGetUniformLocation(program, "material.ambient");
    mDiffLoc = glGetUniformLocation(program, "material.diffuse");
    mSpecLoc = glGetUniformLocation(program, "material.specular");
    mShiLoc = glGetUniformLocation(program, "material.shininess");

    glProgramUniform4fv(program, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(program, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(program, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(program, specLoc, 1, lightSpecular);
    glProgramUniform3fv(program, posLoc, 1, lightPos);

    glProgramUniform4fv(program, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(program, mDiffLoc, 1, matDif);
    glProgramUniform4fv(program, mSpecLoc, 1, matSpe);
    glProgramUniform1f(program, mShiLoc, matShi);
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

    auto t_vert = myTorus.getVertices();
    auto t_text = myTorus.getTextCoords();
    auto t_norm = myTorus.getNormals();
    auto t_ind = myTorus.getIndices();

    std::vector<float> t_flat_vert;
    std::vector<float> t_flat_text;
    std::vector<float> t_flat_norm;

    int t_numIndices = myTorus.getNumIndices();
    for (int i = 0; i < t_numIndices; i++) {
        t_flat_vert.push_back((t_vert[i]).x);
        t_flat_vert.push_back((t_vert[i]).y);
        t_flat_vert.push_back((t_vert[i]).z);

        t_flat_text.push_back((t_text[i]).s);
        t_flat_text.push_back((t_text[i]).t);

        t_flat_norm.push_back((t_norm[i]).x);
        t_flat_norm.push_back((t_norm[i]).y);
        t_flat_norm.push_back((t_norm[i]).z);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, t_flat_vert.size() * 4, &t_flat_vert[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, t_flat_text.size() * 4, &t_flat_text[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, t_flat_norm.size() * 4, &t_flat_norm[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, t_ind.size() * 4, &t_ind[0], GL_STATIC_DRAW);
}


void init(GLFWwindow* window) {
    renderingProgram1 = createShaderProgram("../glsl/phong/vertShader1.glsl", "../glsl/phong/fragShader1.glsl");
    renderingProgram2 = createShaderProgram("../glsl/phong/vertShader2.glsl", "../glsl/phong/fragShader2.glsl");
    renderingProgram3 = createShaderProgram("../glsl/phong/vertShader2.glsl", "../glsl/singleColor.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
    brickTexture = loadTexture("../assets/Studio522Dolphin/Dolphin_HighPolyUV.png");
    setup_vertices();
    setupShadowBuffers(window);
}

void passOne() {
    glUseProgram(currentProgram);
    sLoc = glGetUniformLocation(currentProgram, "shadowMVP");

    mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
    mMat = glm::rotate(mMat, toRadians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    shadowMVP1 = lightPMatrix * lightVMatrix * mMat;
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    mMat = glm::rotate(mMat, toRadians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(0), glm::vec3(0.0f, 1.0f, 0.0f));

    shadowMVP1 = lightPMatrix * lightVMatrix * mMat;
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));


    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void passTwo() {
    glUseProgram(currentProgram);
    vLoc = glGetUniformLocation(currentProgram, "mv_matrix");
    projLoc = glGetUniformLocation(currentProgram, "proj_matrix");
    nLoc = glGetUniformLocation(currentProgram, "norm_matrix");
    sLoc = glGetUniformLocation(currentProgram, "shadowMVP");
    pMat = glm::perspective(0.5f, aspect, 0.1f, 1000.0f);

    mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
    mMat = glm::rotate(mMat, toRadians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvMat = vMat *  mMat * sMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));

    shadowMVP2 = b * lightPMatrix * lightVMatrix * mMat;

    currentLightPos = glm::vec3(initialLightLoc);
    installLights(currentProgram,vMat);


    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

    ////////////////////////////////////////// torus /////////////////////

    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    mMat = glm::rotate(mMat, toRadians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, toRadians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

    currentLightPos = glm::vec3(initialLightLoc);
    installLights(currentProgram,vMat);

    mvMat =  vMat *  mMat * sMat ;
    invTrMat = glm::transpose(glm::inverse(mvMat));
    shadowMVP2 = b * lightPMatrix * lightVMatrix * mMat;


    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void display(GLFWwindow* window, double currentTime) {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.2, 0.5,0.3, 1.0);

    aspect = (float) width / (float) height;

    currentLightPos = glm::vec3(initialLightLoc);

    lightVMatrix = glm::lookAt(currentLightPos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
    lightPMatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    currentProgram = renderingProgram1;
    passOne();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // this line will cause blinking, don't know why
//    glDrawBuffer(GL_FRONT);

    glStencilFunc(GL_ALWAYS, 1, 0xff);
    glStencilMask(0xff);
    sMat = glm::mat4(1.0f);
    currentProgram = renderingProgram2;
    passTwo();


    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    sMat = glm::scale(sMat, glm::vec3(1.04f, 1.04f, 1.04f));
    currentProgram = renderingProgram3;
    passTwo();
    glStencilMask(0xff);
    glEnable(GL_DEPTH_TEST);
}


int main() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
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
