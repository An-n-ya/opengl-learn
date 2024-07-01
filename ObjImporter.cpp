#include "ObjImporter.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Model::Model(const char *filename) {
    ObjImporter importer = ObjImporter();
    importer.parseObj(filename);
    numVertices = importer.getNumVertices();
    cout << "loaded " << numVertices << " vertices." << endl;
    auto verts = importer.getVertices();
    auto tcs = importer.getTextCoords();
    auto norms = importer.getNormals();

    for (int i = 0; i < numVertices; i++) {
        vertices.emplace_back(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]);
        textCoords.emplace_back(tcs[i * 2], tcs[i * 2 + 1]);
        normals.emplace_back(norms[i * 3], norms[i * 3 + 1], norms[i * 3 + 2]);
    }
}

int Model::getNumVertices() {return numVertices;}
vector<glm::vec3> Model::getVertices()  {return vertices;}
vector<glm::vec2> Model::getTextCoords() {return textCoords;}
vector<glm::vec3> Model::getNormals(){return normals;}


ObjImporter::ObjImporter() { }

void ObjImporter::parseObj(const char *filename) {
    float x, y, z;
    string content;
    ifstream fileStream(filename, ios::in);
    string line;
    while (!fileStream.eof()) {
        getline(fileStream, line);
        if (line.compare(0, 2, "v ") == 0) {
            stringstream ss(line.erase(0, 1));
            ss >> x; ss >> y; ss >> z;
            vertVals.emplace_back(x);
            vertVals.emplace_back(y);
            vertVals.emplace_back(z);
        }
        if (line.compare(0, 2, "vt") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y;
            stVals.emplace_back(x);
            stVals.emplace_back(y);
        }
        if (line.compare(0, 2, "vn") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y; ss >> z;
            normVals.emplace_back(x);
            normVals.emplace_back(y);
            normVals.emplace_back(z);
        }
        // triangles

        if (line.compare(0, 1, "f") == 0) {
            string oneCorner, v, t, n;
            stringstream ss(line.erase(0, 2));
            for (int i = 0; i < 3; i++) {
                getline(ss, oneCorner, ' ');
                stringstream oneCornerSS(oneCorner);
                getline(oneCornerSS, v, '/');
                getline(oneCornerSS, t, '/');
                getline(oneCornerSS, n, '/');

                int vertRef = (stoi(v) - 1) * 3;
                int tcRef = (stoi(t) - 1) * 2;
                int normRef = (stoi(n) - 1) * 3;


                for (int j = 0; j < 3; j++) {
                    triangleVerts.emplace_back(vertVals[vertRef + j]);
                }
                for (int j = 0; j < 2; j++) {
                    textureCoords.emplace_back(stVals[tcRef + j]);
                }
                for (int j = 0; j < 3; j++) {
                    normals.emplace_back(normVals[normRef + j]);
                }


            }
        }
    }
}

int ObjImporter::getNumVertices() {return (int)(triangleVerts.size() / 3);}
vector<float> ObjImporter::getVertices() {return triangleVerts;}
vector<float> ObjImporter::getTextCoords()  {return textureCoords;}
vector<float> ObjImporter::getNormals()  {return normals;}

