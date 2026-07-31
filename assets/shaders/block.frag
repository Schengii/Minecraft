#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in float Light;
in vec3 FragPos;

uniform sampler2D u_Texture;
uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform vec3 u_SkyColor;

void main() {
    // Basic Directional Light + Ambient + Face Light
    vec3 norm = normalize(Normal);
    vec3 sunDir = normalize(u_SunDirection);
    float diff = max(dot(norm, sunDir), 0.0);
    
    vec3 ambient = 0.4 * u_SkyColor;
    vec3 diffuse = diff * u_SunColor;
    vec3 lighting = (ambient + diffuse) * Light;

    vec4 texColor = texture(u_Texture, TexCoord);
    
    // Fallback colored render if texture alpha is clear
    if (texColor.a < 0.1) {
        // Procedural face colors based on normal
        vec3 color = vec3(0.45, 0.7, 0.25); // Grass top
        if (norm.y < -0.5) color = vec3(0.4, 0.3, 0.2); // Dirt bottom
        else if (abs(norm.x) > 0.5 || abs(norm.z) > 0.5) color = vec3(0.5, 0.4, 0.3); // Dirt side
        
        FragColor = vec4(color * lighting, 1.0);
    } else {
        FragColor = vec4(texColor.rgb * lighting, texColor.a);
    }
}
