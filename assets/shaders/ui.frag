#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_Color;
uniform bool u_UseTexture;
uniform sampler2D u_Texture;

void main() {
    if (u_UseTexture) {
        vec4 texColor = texture(u_Texture, TexCoord);
        FragColor = texColor * u_Color;
    } else {
        FragColor = u_Color;
    }
}
