#include "lighting_technique.h"

// ��������� ������
// ������ � ������ - �������, ���������� ��������, � �������
// ������� - ����������������� ���������� �������� � ������ �������

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                     \n\
}";

// ����������� ������
// �� ����� - ����������������� �������� ��������, ������ �������
// ����� - ���������� ���� ������� FragColor

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    vec3 Direction;                                                                 \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);    \n\
                                                                                    \n\
    vec4 DiffuseColor;                                                              \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        \n\
                       gDirectionalLight.DiffuseIntensity *                         \n\
                       DiffuseFactor;                                               \n\
    }                                                                               \n\
    else{                                                                           \n\
        DiffuseColor = vec4(0,0,0,0);                                               \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                (AmbientColor + DiffuseColor);                                      \n\
}";


// dot - ��������� ������������ ��������. ��� ������ - ��� ������ ���� -> ��� ���� ������ ���� �������
// ���� ������� = ������� �������� * ���� ����� * ������������� �������� + ����������� + ����������� ��������� 

LightingTechnique::LightingTechnique() {
}

bool LightingTechnique::Init()
{
    if (!Technique::Init())
    {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, pVS))  // ���������� ���������� �������
    {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, pFS)) // ���������� ������������ �������
    {
        return false;
    }

    if (!Finalize())
    {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP"); // uniform ���������� ������� �������
    m_WorldMatrixLocation = GetUniformLocation("gWorld"); // ��������� ������� ������� ���������� ������� �������������
    // ������ �����, ����� �������� � ����������� ������ ������ �������� ��� ������� � ����� �������
    m_samplerLocation = GetUniformLocation("gSampler"); // ��������� ������� �������� uniform ����������
    // uniform ���������� �������� ����� (���� � �������������)
    m_dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Color");
    m_dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.AmbientIntensity");
    // uniform ���������� ���������� ����� (���� � �������������)
    m_dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
    m_dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.DiffuseIntensity");
    
    // ���� ���� �� ���� �� ��������
    if (m_dirLightLocation.AmbientIntensity == 0xFFFFFFFF ||
        m_WVPLocation == 0xFFFFFFFF ||
        m_WorldMatrixLocation == 0xFFFFFFFF ||
        m_samplerLocation == 0xFFFFFFFF ||
        m_dirLightLocation.Color == 0xFFFFFFFF ||
        m_dirLightLocation.DiffuseIntensity == 0xFFFFFFFF ||
        m_dirLightLocation.Direction == 0xFFFFFFFF) {
        return false;
    }

    return true;
}

void LightingTechnique::SetWVP(const Matrix4f& WVP)
{
    // �������� � ������� ���������� ������� �������������
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void LightingTechnique::SetWorldMatrix(const Matrix4f& WorldInverse)
{
    glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, (const GLfloat*)WorldInverse.m);
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    // ������������� ������� ������� ��������,
    // ������� �� ���������� ������������ ������ �������� uniform-���������� � �������
    glUniform1i(m_samplerLocation, TextureUnit);
}

// �������� ��������������� �������� ������ � �������� ������������� ����� � ������� ����������
void LightingTechnique::SetDirectionalLight(const DirectionLight& Light)
{
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}