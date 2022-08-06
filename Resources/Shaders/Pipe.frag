#version 430 core

struct Node {
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Light {
    vec4 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
};

in vec3 fs_position;
in vec3 fs_normal;

uniform vec3 cameraPosition;
uniform Node node;
uniform Light light;

out vec4 out_color;

void main()
{
    // Ambient
    float ambient = light.ambient * node.ambient;

    // Diffuse
    vec3 norm = normalize(fs_normal);
    vec3 lightDir = normalize(light.position - fs_position);
    float diff = max(dot(norm, lightDir), 0.0);
    float diffuse = light.diffuse * (diff * node.diffuse);

    // Specular
    vec3 viewDir = normalize(cameraPosition - fs_position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
    float specular = light.specular * (spec * node.specular);

    out_color = (specular + ambient + diffuse) * node.color * light.color;
}
