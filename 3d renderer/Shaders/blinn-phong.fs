#version 450 core
layout (location = 0)out vec4 FragColor;
layout (location = 1)out vec4 BrightColor;

in VS_OUT
{
    vec2 Tex;
    vec3 FragPos;
    mat3 TBN;
} fs_in;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    
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
uniform float far_plane;

vec3 CalculatePoints(PointLight light, vec3 normal,vec3 fragpos, vec3 viewDir);
vec3 CalculateSpots(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateDir(DirectLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = texture(material.texture_normal1, fs_in.Tex).rgb;
    norm = norm * 2.0 - 1.0;
    norm = normalize(fs_in.TBN * norm);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 Output = CalculateDir(direct, norm, viewDir);
    for(int i = 0; i < point_count; i++)
    {
      Output += CalculatePoints(points[i], norm, fs_in.FragPos, viewDir);
    }
    for(int i = 0; i < spot_count; i++)
    {
      Output += CalculateSpots(spots[i], norm, fs_in.FragPos, viewDir);
    }

    float brightness = dot(Output, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) 
        BrightColor = vec4(Output, 1.0f);
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(Output, 1.0f);
}

vec3 CalculatePoints(PointLight light, vec3 normal,vec3 fragpos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragpos);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    float distance = length(light.position - fragpos);
    float attenuation = 1.0/(light.constant + light.linear * distance  + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.Tex)).rgb;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return  (ambient + diffuse + specular);
}

vec3 CalculateDir(DirectLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;

    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.Tex)).rgb;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpots(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance  + light.quadratic * (distance * distance));
    
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.Tex)).rgb;
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.Tex)).rgb;

    float theta = dot(lightDir, normalize(-light.direction));
    float epilson = (light.cutoff - light.outerCutoff);
    float intensity = clamp((theta - light.outerCutoff) / epilson, 0.0, 1.0);

    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;
    ambient *= attenuation * attenuation;
    
    
    return (ambient + diffuse + specular);
}