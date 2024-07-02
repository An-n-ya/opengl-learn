#version 430
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec2 tc;
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

layout (binding = 0) uniform sampler2D samp;


void main(void) {
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(varyingVertPos);

    vec3 R = normalize(reflect(-L, N));

    vec4 ambientLight = globalAmbient * material.ambient + light.ambient * material.ambient;
    vec4 diffuseLight = vec4(light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0), 1.0);
    vec4 specularLight = vec4(light.specular.xyz * material.specular.xyz * pow(max(dot(R, V), 0.0),
        material.shininess), 1.0);

//    vec4 textureColor = texture(samp, tc);
    color =  (ambientLight + diffuseLight) + specularLight;
}