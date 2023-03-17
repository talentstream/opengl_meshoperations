#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

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
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = diff * diffuse * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
        
    vec3 specularColor = spec* specular * lightColor; // assuming bright white light color
    FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}