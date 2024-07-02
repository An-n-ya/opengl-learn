#version 430
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec2 tc;
in vec4 shadow_coord;
out vec4 color;

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

layout (binding = 0) uniform sampler2DShadow shTex;

float lookup(float ox, float oy) {
    float t = textureProj(
        shTex,
        shadow_coord +
        vec4(ox * 0.001 * shadow_coord.w, oy * 0.001 * shadow_coord.w, -0.01, 0.0)
    );
    return t;
}


void main(void) {
    float shadowFactor = 0.0;
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(varyingVertPos);

    vec3 R = normalize(reflect(-L, N));

    float s_width = 6.0;
    vec2 offset = mod(floor(gl_FragCoord.xy), 2.0) * s_width;
    shadowFactor += lookup(-1.5 * s_width + offset.x, 1.5 * s_width - offset.y);
    shadowFactor += lookup(-1.5 * s_width + offset.x, -0.5 * s_width - offset.y);
    shadowFactor += lookup(0.5 * s_width + offset.x, 1.5 * s_width - offset.y);
    shadowFactor += lookup(0.5 * s_width + offset.x, -0.5 * s_width - offset.y);
    shadowFactor = shadowFactor / 4.0;

//    float notInShadow = textureProj(shTex, shadow_coord);

    vec4 ambientLight = globalAmbient * material.ambient + light.ambient * material.ambient;
    vec4 diffuseLight = vec4(light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0), 1.0);
    vec4 specularLight = vec4(light.specular.xyz * material.specular.xyz * pow(max(dot(R, V), 0.0),
        material.shininess), 1.0);

    color = ambientLight + shadowFactor * (diffuseLight + specularLight);
//    if (notInShadow != 0.0) {
//        color =  (ambientLight + diffuseLight) + specularLight;
//    } else {
//        color = ambientLight;
//    }
}