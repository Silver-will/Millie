#version 330 core

in VS_OUT
{
    vec2 Tex;
    vec3 Norm;
    vec3 FragPos;
} fs_in;
out vec4 fragColor;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
}
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
    vec3 ambient
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

}
#define MAX_POINT_LIGHT;

uniform PointLight points[MAX_POINT_LIGHT];
uniform DirectLight directLight;
uniform SpotLight spot;
uniform Material material;
uniform int PointLight count;
uniform vec3 viewPos;

float CalculatePoints(PointLight point, vec3 normal,vec3 fragpos vec3 viewDir, int count);
float CalculateSpot(SpotLight spot, vec3 normal, vec3 fragPos, vec3 viewDir)
float CalculateDirs(DirectLight dir, vec3 normal, vec3 viewDir);
void main()
{
    vec3 norm = normalize(fs_in.Norm);
    vec3 view = viewPos - fs_in.FragPos;

    fragColor = texture(texture_diffuse1, Tex);
}