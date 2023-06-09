#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    // sampler2D specular;
    // float shininess;
};



struct DirLight {   // Directional light e.g the sun
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;


struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float bias;
};
#define MAX_POINT_LIGHTS 10
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;
uniform samplerCube depthMaps[MAX_POINT_LIGHTS];

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform float far_plane;

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow);
float calcShadow(PointLight light, vec3 normal, vec3 fragPos, int depthMapIndex);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
 
    

    // calculate point light
    vec4 result = vec4(0.0f);
    for (int i = 0; i < pointLightCount; i++) {
        float shadow = calcShadow(pointLights[i], norm, FragPos, i);
        result += calcPointLight(pointLights[i], norm, viewDir, shadow);
    }

    FragColor = result;
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec4 tx = texture(material.diffuse, TexCoords).rgba;
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
        
    vec3 result = (ambient + (1.0 - shadow)) * diffuse;
    vec4 final = vec4(result, tx.a);
    return final;
}

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    // ambient
    vec4 tx = texture(material.diffuse, TexCoords).rgba;
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
            
    vec3 result = ambient + diffuse;
    vec4 final = vec4(result, tx.a);
    return final;
}

float calcShadow(PointLight light, vec3 normal, vec3 fragPos, int depthMapIndex) {
    vec3 fragToLight = fragPos - light.position;
    float closestDepth = texture(depthMaps[depthMapIndex], fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    vec3 lightDir = normalize(light.position - FragPos);
    //float bias = max(0.5 * (1.0 - dot(normal, lightDir)), 0.005);  
    float bias = light.bias;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // vec3 fragToLight = fragPos - light.position;

    // float currentDepth = length(fragToLight);
    
    // // experiment with vals
    // float shadow = 0.0;
    // float bias = 0.15;
    
    
    //int samples = 20;
    // float viewDistance = length(viewPos - fragPos);
    // float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    // for (int i = 0; i < samples; ++i) {
    //     float closestDepth = texture(depthMap, fragToLight, gridSamplingDisk[i] * diskRadius).r;
    //     closestDepth *= far_plane;
    //     if (currentDepth - bias > closestDepth) {
    //         shadow += 1.0;
    //     }
    // }
    // shadow /= float(samples);

    return shadow;
}