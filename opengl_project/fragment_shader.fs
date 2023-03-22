#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 FragNorm;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

void main()
{
    
    // ambient
    float ambientStrength = 0.5f;
    vec3 ambientColor = ambientStrength * ambient * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(FragNorm);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = diff * diffuse * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
        
    vec3 specularColor = spec * specular * lightColor; // assuming bright white light color
    FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}