#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aLight;
layout (location = 4) in float aAO;

out vec2 TexCoord;
out vec3 Normal;
out float Light;
out float AO;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;
uniform float u_Time;

void main() {
    vec3 pos = aPos;
    
    // Wind Flutter simulation for leaves and water ripples
    if (aNormal.y > 0.4) {
        float wave = sin(u_Time * 2.5 + aPos.x * 2.0 + aPos.z * 2.0) * 0.035;
        pos.x += wave;
        pos.z += wave * 0.6;
    }
    
    vec4 worldPos = u_Model * vec4(pos, 1.0);
    FragPos = worldPos.xyz;
    FragPosLightSpace = u_LightSpaceMatrix * worldPos;
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    Light = aLight;
    AO = aAO;
    
    gl_Position = u_Projection * u_View * worldPos;
}
