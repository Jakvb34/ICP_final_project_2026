#version 410 core

uniform sampler2D tex0;

uniform vec3 ambient_intensity = vec3(0.15, 0.15, 0.15);
uniform vec3 directional_light_direction = vec3(-0.3, -1.0, -0.4);
uniform vec3 directional_light_color = vec3(0.35, 0.35, 0.35);
uniform vec3 specular_material = vec3(1.0, 1.0, 1.0);
uniform float specular_shinines = 32.0;

uniform vec3 point_light_positions[3];
uniform vec3 point_light_colors[3];

uniform vec3 spot_position;
uniform vec3 spot_direction;

uniform vec3 spot_color = vec3(1.0, 1.0, 1.0);

uniform float spot_cutoff = 0.95;

in VS_OUT {
    vec3 N;
    vec3 P;
    vec3 V;
    vec2 texCoord;
} fs_in;

out vec4 FragColor;

void main() {
    vec3 N = normalize(fs_in.N);
    vec3 V = normalize(fs_in.V);

    vec3 texColor = texture(tex0, fs_in.texCoord).rgb;

    vec3 finalColor = texColor * ambient_intensity;
    vec3 DL = normalize(-directional_light_direction);
    float directionalDiffuse = max(dot(N, DL), 0.0);
    finalColor += directionalDiffuse * texColor * directional_light_color;

    for (int i = 0; i < 3; ++i) {
        vec3 L = normalize(point_light_positions[i] - fs_in.P);
        vec3 R = reflect(-L, N);

        float distanceToLight = length(point_light_positions[i] - fs_in.P);
        float attenuation = 1.0 / (1.0 + 0.25 * distanceToLight * distanceToLight);

        float diffuseStrength = max(dot(N, L), 0.0);
        vec3 diffuse = diffuseStrength * texColor * point_light_colors[i];

        float specularStrength =
            pow(max(dot(R, V), 0.0), specular_shinines);

        vec3 specular =
            specularStrength * specular_material * point_light_colors[i];

        finalColor += attenuation * (diffuse + specular);
    }
        vec3 spotDir = normalize(spot_direction);
        vec3 toSpot = normalize(spot_position - fs_in.P);

       float theta = dot(normalize(-toSpot), spotDir);

        if (theta > spot_cutoff) {
         vec3 L = normalize(spot_position - fs_in.P);
         vec3 R = reflect(-L, N);

         float diffuseStrength = max(dot(N, L), 0.0);
         vec3 diffuse = diffuseStrength * texColor * spot_color;

         float specularStrength =
         pow(max(dot(R, V), 0.0), specular_shinines);

         vec3 specular =
         specularStrength * specular_material * spot_color;

         finalColor += diffuse + specular;
       }
    FragColor = vec4(finalColor, 1.0);
}