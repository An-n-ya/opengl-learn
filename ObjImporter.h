#include <vector>
#include <glm/glm.hpp>
using namespace std;

class Model {
private:
    int numVertices;
    vector<glm::vec3> vertices;
    vector<glm::vec2> textCoords;
    vector<glm::vec3> normals;
public:
    Model(const char *filename);
    int getNumVertices();
    vector<glm::vec3> getVertices();
    vector<glm::vec2> getTextCoords();
    vector<glm::vec3> getNormals();
};

class ObjImporter {
private:
    vector<float> vertVals;
    vector<float> stVals;
    vector<float> normVals;

    vector<float> triangleVerts;
    vector<float> textureCoords;
    vector<float> normals;
public:
    ObjImporter();
    void parseObj(const char *filename);
    int getNumVertices();
    vector<float> getVertices();
    vector<float> getTextCoords();
    vector<float> getNormals();

};

