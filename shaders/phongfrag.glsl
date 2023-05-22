#version 460 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    // sampler2D specular; TODO: re-enable specular component
    // float shininess;
};
uniform Material material;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;

vec4 calcDirLight();

void main(){
    FragColor = calcDirLight();
}

vec4 calcDirLight() {
    vec4 tx4 = texture(material.diffuse, TexCoords).rgba;
    vec3 tx3 = texture(material.diffuse, TexCoords).rgb;
    // ambient
    vec3 ambient = dirLight.ambient * tx3;
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * tx3;
    // specular... what to do?

    vec3 result = ambient + diffuse;
    vec4 final = vec4(result, tx4.a);
    return final;
}

