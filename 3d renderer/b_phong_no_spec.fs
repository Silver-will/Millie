#version 450 core
in VS_OUT
{
    vec2 Tex;
    vec3 Norm;
    vec3 FragPos;
} fs_in;

struct Material
{
    sampler2D texture_diffuse1;
    float shininess;
};
struct DirectLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};
struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    samplerCube depthMap;
    bool shadow;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;

    bool shadow;
};

#define MAX_POINT_LIGHT 50
#define MAX_SPOT_LIGHT 50

uniform PointLight points[MAX_POINT_LIGHT];
uniform DirectLight direct;
uniform SpotLight spots[MAX_SPOT_LIGHT];
uniform Material material;
uniform int point_count;
uniform int spot_count;
uniform vec3 viewPos;
uniform float gamma;
uniform float far_plane;
uniform vec3 specular_component;
out vec4 fragColor;

vec3 CalculatePoints(PointLight light, vec3 normal,vec3 fragpos, vec3 viewDir);
vec3 CalculateSpots(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateDir(DirectLight light, vec3 normal, vec3 viewDir);
float ShadowCalculation(vec3 fragPos, PointLight light);

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void main()
{
    //vec3 norm = normalize(fs_in.Norm);
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 Output = CalculateDir(direct, norm, viewDir);
    //vec3 Output = vec3(0.0f);
    //for(int i = 0; i < point_count; i++)
    //{
      //Output += CalculatePoints(points[i], norm, fs_in.FragPos, viewDir);
    //}
    //for(int i = 0; i < spot_count; i++)
    //{
      //Output += CalculateSpots(spots[i], norm, fs_in.FragPos, viewDir);
    //}
    //Output = pow(Output, vec3(1.0/gamma));
    //fragColor = vec4(Output, 1.0f);
}

vec3 CalculatePoints(PointLight light, vec3 normal,vec3 fragpos, vec3 viewDir)
{
    vec3 lightDir = normalize(fragpos - light.position);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    float distance = length(light.position - fragpos);
    float attenuation = 1.0/(light.constant + light.linear * distance  + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 specular = light.specular * (spec * specular_component);

    float shadow = light.shadow ?  ShadowCalculation(fragpos, light) : 0.0;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (specular);
}

vec3 CalculateDir(DirectLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 specular = light.specular * (spec * specular_component);

    return (ambient + diffuse + specular);
}

vec3 CalculateSpots(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(fragPos - light.position);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance  + light.quadratic * (distance * distance));
    
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;
    vec3 specular = light.specular * (spec * specular_component);

    float theta = dot(lightDir, normalize(-light.direction));
    float epilson = (light.cutoff - light.outerCutoff);
    float intensity = clamp((theta - light.outerCutoff) / epilson, 0.0, 1.0);

    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;
    ambient *= attenuation * attenuation;
    
    
    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec3 fragPos, PointLight light)
{
    vec3 fragToLight = fragPos - light.position;

    float currentDepth = length(fragToLight);
    float shadow = 0.0f;
    float bias = 0.05f;
    int samples = 20;
    float viewDistance = length(viewPos - light.position);
    float diskRadius = (1.0 + (viewDistance/ far_plane)) /  25.0;
    for(int i = 0; i < samples; i++)
    {
        float closestDepth = texture(light.depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;

}