#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aLight;

out vec2 TexCoord;
out vec3 Normal;
out float Light;
out vec3 FragPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    Light = aLight;
    
    gl_Position = u_Projection * u_View * worldPos;
}
