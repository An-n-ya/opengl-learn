#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Torus {
private:
    int numVertices;
    int numIndices;
    float inner;
    float outer;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textCoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> sTangents;
    std::vector<glm::vec3> tTangents;
    void init(float, float, int);
    float toRadians(float degrees);
public:
    Torus();
    Torus(float innerRadius, float outerRadius, int prec);
    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTextCoords();
    std::vector<glm::vec3> getNormals();
};
