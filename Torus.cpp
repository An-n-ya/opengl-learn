#include "Torus.h"
using namespace std;

Torus::Torus() {
    init(0.5f, 0.2f, 48);
}

Torus::Torus(float innerRadius, float outerRadius, int prec) {
    init(innerRadius, outerRadius, prec);
}

float Torus::toRadians(float degrees) {
    return (degrees) * 2.0f * 3.1415926f / 360.0f;
}

void Torus::init(float innerRadius, float outerRadius, int prec) {
    inner = innerRadius;
    outer = outerRadius;
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;

    for (int i = 0; i < numVertices; i++) { vertices.emplace_back(); }
    for (int i = 0; i < numVertices; i++) { textCoords.emplace_back(); }
    for (int i = 0; i < numVertices; i++) { normals.emplace_back(); }
    for (int i = 0; i < numVertices; i++) { sTangents.emplace_back(); }
    for (int i = 0; i < numVertices; i++) { tTangents.emplace_back(); }
    for (int i = 0; i < numIndices; i++) { indices.emplace_back(); }

    // calculate first ring
    for (int i = 0; i < prec + 1; i++) {
        float amt = toRadians(i*360.0f / prec);

        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 initPos(rMat * glm::vec4(0.0f, outer, 0.0f, 1.0f));

        vertices[i] = glm::vec3(initPos + glm::vec3(inner, 0.0f, 0.0f));
        textCoords[i] = glm::vec2(0.0f, ((float)i / (float)prec));

        rMat = glm::rotate(glm::mat4(1.0f), amt + (3.14159f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

        sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));
        normals[i] = glm::cross(tTangents[i], sTangents[i]);
    }
    // rotate the first ring about Y to get the other rings
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int i = 0; i < prec + 1; i++) {
            float amt = (float)toRadians((float)ring * 360.0f / (prec));

            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            vertices[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(vertices[i], 1.0f));

            textCoords[ring*(prec + 1) + i] = glm::vec2((float)ring*2.0f / (float)prec, textCoords[i].t);

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            sTangents[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(sTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            tTangents[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(tTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            normals[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(normals[i], 1.0f));
        }
    }
    // calculate triangle indices
    for (int ring = 0; ring < prec; ring++) {
        for (int i = 0; i < prec; i++) {
            indices[((ring*prec + i) * 2) * 3 + 0] = ring*(prec + 1) + i;
            indices[((ring*prec + i) * 2) * 3 + 1] = (ring + 1)*(prec + 1) + i;
            indices[((ring*prec + i) * 2) * 3 + 2] = ring*(prec + 1) + i + 1;
            indices[((ring*prec + i) * 2 + 1) * 3 + 0] = ring*(prec + 1) + i + 1;
            indices[((ring*prec + i) * 2 + 1) * 3 + 1] = (ring + 1)*(prec + 1) + i;
            indices[((ring*prec + i) * 2 + 1) * 3 + 2] = (ring + 1)*(prec + 1) + i + 1;
        }
    }

}

int Torus::getNumVertices() {return numVertices;}
int Torus::getNumIndices() {return numIndices;}
std::vector<int> Torus::getIndices() {return indices;}
std::vector<glm::vec3> Torus::getVertices() {return vertices;}
std::vector<glm::vec2> Torus::getTextCoords() {return textCoords;}
std::vector<glm::vec3> Torus::getNormals() {return normals;}
