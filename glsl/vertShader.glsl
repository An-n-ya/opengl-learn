#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textCord;
layout (binding = 0) uniform sampler2D samp;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf;

out vec2 tc;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);
mat4 buildScale(float x, float y, float z);

void main(void) {
    float i = gl_InstanceID + tf;
    mat4 localRotX = buildRotateX(i);
    mat4 localRotY = buildRotateY(i);
    mat4 localRotZ = buildRotateZ(0);

    mat4 s_matrix = buildScale(1.5, 1.5, 1.5);

    mat4 newM_matrix = localRotX * localRotY * localRotZ;
    mat4 mv_matrix = v_matrix * newM_matrix;
    gl_Position = proj_matrix *  mv_matrix * s_matrix * vec4(position, 1.0);
    tc = textCord;
}

//void main(void) {
//    float i = gl_InstanceID + tf;
//
//
//    float a = sin(203.0 * i / 8000.0) * 400.0;
//    float b = sin(301.0 * i / 4000.0) * 400.0;
//    float c = sin(400.0 * i / 6000.0) * 400.0;
//
//    mat4 localRotX = buildRotateX(1000 * i);
//    mat4 localRotY = buildRotateY(1000 * i);
//    mat4 localRotZ = buildRotateZ(1000 * i);
//    mat4 localTrans = buildTranslate(a, b, c);
//
//    mat4 newM_matrix = localTrans * localRotX * localRotY * localRotZ;
//    mat4 mv_matrix = v_matrix * newM_matrix;
//
//
//
//    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
//    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
//}

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