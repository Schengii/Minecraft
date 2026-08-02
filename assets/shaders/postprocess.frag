#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_ScreenTexture;
uniform bool u_IsUnderwater;

void main() {
    vec3 col = texture(u_ScreenTexture, TexCoords).rgb;

    // Underwater refraction and bluish fog tint
    if (u_IsUnderwater) {
        vec2 distortion = vec2(sin(TexCoords.y * 30.0) * 0.003, cos(TexCoords.x * 30.0) * 0.003);
        col = texture(u_ScreenTexture, TexCoords + distortion).rgb;
        col = mix(col, vec3(0.05, 0.25, 0.65), 0.45);
    }

    // Exposure & Reinhard Tonemapping for HDR
    vec3 mapped = col / (col + vec3(1.0));
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
