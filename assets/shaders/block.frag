#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Light;
in float AO;
in vec3 FragPos;

uniform sampler2D u_Texture;
uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_SkyColor;
uniform float u_AmbientLight;
uniform bool u_IsUnderwater;

// Dynamic Handheld Light Uniforms
uniform vec3 u_PlayerPos;
uniform bool u_HasHandheldLight;

void main() {
    vec3 norm = normalize(Normal);
    vec3 sunDir = normalize(u_SunDirection);
    float diff = max(dot(norm, sunDir), 0.0);
    
    vec3 ambient = u_AmbientLight * u_SkyColor;
    vec3 diffuse = diff * u_SunColor;

    // Smooth Vertex Ambient Occlusion factor
    float aoFactor = clamp(AO, 0.2, 1.0);
    vec3 lighting = (ambient + diffuse) * Light * aoFactor;

    // Handheld Light calculation
    if (u_HasHandheldLight) {
        float dist = length(FragPos - u_PlayerPos);
        if (dist < 14.0) {
            float handLight = clamp(1.0 - (dist / 14.0), 0.0, 1.0);
            lighting += vec3(handLight * 0.85, handLight * 0.70, handLight * 0.45); // Warm torch glow
        }
    }

    vec4 texColor = texture(u_Texture, TexCoord);
    
    vec3 baseColor;
    if (texColor.a < 0.1) {
        baseColor = vec3(0.45, 0.7, 0.25);
        if (norm.y < -0.5) baseColor = vec3(0.4, 0.3, 0.2);
        else if (abs(norm.x) > 0.5 || abs(norm.z) > 0.5) baseColor = vec3(0.5, 0.4, 0.3);
    } else {
        baseColor = texColor.rgb;
    }

    vec3 finalColor = baseColor * lighting;

    // Underwater Fog & Tint Effect
    if (u_IsUnderwater) {
        vec3 waterTint = vec3(0.1, 0.3, 0.7);
        finalColor = mix(finalColor, waterTint, 0.55);
    }

    FragColor = vec4(finalColor, texColor.a < 0.1 ? 1.0 : texColor.a);
}
