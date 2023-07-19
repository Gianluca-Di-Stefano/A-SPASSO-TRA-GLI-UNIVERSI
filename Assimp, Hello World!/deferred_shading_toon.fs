#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform bool enableOutline = true;
uniform float outlineIntensity;
uniform vec3 outlineColor;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);
    float scale = 1.0; //scala della sfera per aumentare l intensità
    float radius = 0.05;
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos)/ lights[i].Radius * radius;
        if(distance < lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);

            vec3 toonDiffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color * scale;

            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            float specularLevel = spec; // puoi quantizzare il valore speculare come desideri
            vec3 toonSpecular = specularLevel * Specular * lights[i].Color;

            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            toonDiffuse *= attenuation;
            toonSpecular *= attenuation;

            lighting += toonDiffuse + toonSpecular;
        }
    }  
    FragColor = vec4(lighting, 1.0);
}
