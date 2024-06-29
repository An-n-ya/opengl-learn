#include <vector>
#include <glm/glm.hpp>
class Sphere {
private:
    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textCoords;
    std::vector<glm::vec3> normals;
    void init(int);
    float toRadians(float degrees);
public:
    Sphere(int prec);
    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTextCoords();
    std::vector<glm::vec3> getNormals();
};