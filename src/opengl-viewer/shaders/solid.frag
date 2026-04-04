#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 materialColor;
uniform float metalness;
uniform float roughness;

uniform vec3 ambientColor;
uniform vec3 directionalColor;
uniform vec3 lightDirection;

void main()
{
    // 环境光
    vec3 ambient = ambientColor * materialColor;
    
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = directionalColor * diff * materialColor;
    
    // 镜面反射
    vec3 viewDir = normalize(vec3(0.0, 0.0, 5.0) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = directionalColor * spec * (0.5 + metalness * 0.5);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}