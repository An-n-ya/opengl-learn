#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textCord;
layout (location = 2) in vec3 normal;
layout (binding = 0) uniform sampler2DShadow shTex;

struct PositionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP;


out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec2 tc;
out vec4 shadow_coord;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);
mat4 buildScale(float x, float y, float z);

void main(void) {
    varyingVertPos = (mv_matrix * vec4(position, 1.0)).xyz;
    varyingLightDir = light.position - varyingVertPos;
    varyingNormal = (norm_matrix * vec4(normal, 1.0)).xyz;
    shadow_coord = shadowMVP * vec4(position, 1.0);


    tc = textCord;
    gl_Position = proj_matrix *  mv_matrix *  vec4(position, 1.0);
}


mat4 buildRotateX(float rad) {
    mat4 res = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, cos(rad), -sin(rad), 0.0,
    0.0, sin(rad), cos(rad), 0.0,
    0.0, 0.0, 0.0, 1.0
    );
    return res;
}
mat4 buildRotateY(float rad) {
    mat4 res = mat4(
    cos(rad), 0.0, sin(rad), 0.0,
    0.0, 1.0, 0.0, 0.0,
    -sin(rad), 0.0, cos(rad), 0.0,
    0.0, 0.0, 0.0, 1.0
    );
    return res;
}
mat4 buildRotateZ(float rad) {
    mat4 res = mat4(
    cos(rad), -sin(rad), 0.0, 0.0,
    sin(rad), cos(rad), 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
    );
    return res;
}
mat4 buildTranslate(float x, float y, float z) {
    mat4 res = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    x, y, z, 1.0
    );
    return res;
}
mat4 buildScale(float x, float y, float z) {
    mat4 res = mat4(
        x, 0.0, 0.0, 0.0,
        0.0, y, 0.0, 0.0,
        0.0, 0.0, z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    return res;
}