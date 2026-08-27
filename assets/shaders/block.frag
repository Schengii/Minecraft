#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Light;
in float AO;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D u_Texture;
uniform sampler2D u_ShadowMap;
uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_SkyColor;
uniform float u_AmbientLight;
uniform bool u_IsUnderwater;

// Dynamic Handheld Light Uniforms
uniform vec3 u_PlayerPos;
uniform bool u_HasHandheldLight;

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 sunDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float bias = max(0.003 * (1.0 - dot(normal, sunDir)), 0.001);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 sunDir = normalize(u_SunDirection);
    vec3 viewDir = normalize(u_PlayerPos - FragPos);

    float diff = max(dot(norm, sunDir), 0.0);
    
    vec3 ambient = u_AmbientLight * u_SkyColor;
    vec3 diffuse = diff * u_SunColor;

    float shadow = CalculateShadow(FragPosLightSpace, norm, sunDir);

    // Smooth Vertex Ambient Occlusion factor
    float aoFactor = clamp(AO, 0.2, 1.0);
    vec3 lighting = (ambient + (1.0 - shadow * 0.70) * diffuse) * Light * aoFactor;

    // Blinn-Phong Specular calculation (sun glint)
    vec3 halfwayDir = normalize(sunDir + viewDir);
    float specFactor = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = (1.0 - shadow) * specFactor * u_SunColor * 0.35 * max(dot(norm, sunDir), 0.0);

    // Dynamic Handheld Torch Light calculation
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

    vec3 finalColor = baseColor * lighting + specular;

    // Atmospheric Distance Fog & Sun In-Scattering
    float fragDist = length(FragPos - u_PlayerPos);
    float fogDensity = u_IsUnderwater ? 0.045 : 0.0075;
    float fogFactor = 1.0 - exp(-pow(fragDist * fogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Sun In-Scattering (God ray glow towards sun)
    float sunScattering = max(dot(-viewDir, sunDir), 0.0);
    vec3 fogColor = mix(u_SkyColor, u_SunColor, pow(sunScattering, 4.0) * 0.45);

    if (u_IsUnderwater) {
        fogColor = vec3(0.08, 0.25, 0.65);
    }

    finalColor = mix(finalColor, fogColor, fogFactor);

    FragColor = vec4(finalColor, texColor.a < 0.1 ? 1.0 : texColor.a);
}
