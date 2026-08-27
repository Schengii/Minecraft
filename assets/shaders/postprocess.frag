#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_ScreenTexture;
uniform bool u_IsUnderwater;
uniform bool u_HasNightVision;
uniform bool u_BloomEnabled;
uniform bool u_SSAOEnabled;

// ACES Filmic Tone Mapping Curve
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec2 uv = TexCoords;
    vec3 col;

    // Underwater wave refraction and deep ocean fog tint
    if (u_IsUnderwater) {
        vec2 distortion = vec2(sin(uv.y * 30.0) * 0.003, cos(uv.x * 30.0) * 0.003);
        col = texture(u_ScreenTexture, uv + distortion).rgb;
        col = mix(col, vec3(0.05, 0.25, 0.65), 0.45);
    } else {
        col = texture(u_ScreenTexture, uv).rgb;
    }

    // Screen-Space Ambient Occlusion (SSAO) Contact Darkening
    if (u_SSAOEnabled) {
        vec2 texel = 1.0 / textureSize(u_ScreenTexture, 0);
        float sample1 = length(texture(u_ScreenTexture, uv + vec2(texel.x * 2.0, 0.0)).rgb);
        float sample2 = length(texture(u_ScreenTexture, uv - vec2(texel.x * 2.0, 0.0)).rgb);
        float sample3 = length(texture(u_ScreenTexture, uv + vec2(0.0, texel.y * 2.0)).rgb);
        float sample4 = length(texture(u_ScreenTexture, uv - vec2(0.0, texel.y * 2.0)).rgb);
        float center = length(col);
        float curvature = max(sample1 + sample2 + sample3 + sample4 - 4.0 * center, 0.0);
        float ssaoFactor = clamp(1.0 - curvature * 0.35, 0.55, 1.0);
        col *= ssaoFactor;
    }

    // Bloom extraction approximation for glowing elements
    if (u_BloomEnabled) {
        vec3 bright = max(col - vec3(0.7), vec3(0.0));
        col += bright * 0.45;
    }

    // Night Vision Potion Effect: Brighten scene and amplify ambient
    if (u_HasNightVision) {
        col = col * 2.2 + vec3(0.08, 0.12, 0.15);
    }

    // Vignette Effect
    float dist = distance(uv, vec2(0.5, 0.5));
    float vignette = smoothstep(0.85, 0.25, dist);
    col *= vignette;

    // ACES Filmic Tone Mapping & Gamma correction
    vec3 mapped = ACESFilm(col);
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
