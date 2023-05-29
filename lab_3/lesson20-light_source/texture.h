#pragma once
#include <string>

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture
{
public:
    // ������� �������� (� ����: 1D, 2D, 3D) � ��� �����
    Texture(GLenum TextureTarget, const std::string& FileName);

    Texture(GLenum TextureTarget);

    //-----------------------------------------------------------------------------------
    // �������� �������� �� �����
    bool Load();
    // ��������� ������ ������ � ��������� ������������ �������� � ������
    /*void Load(u32 BufferSize, void* pData);
    void Load(const std::string& Filename);*/
    //-----------------------------------------------------------------------------------

    // �������� �����
    void LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // �������� � ����������� ������
    void Bind(GLenum TextureUnit);

    // ��������� ������� ��������
    void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

private:
    void LoadInternal(void* image_data);

    std::string m_fileName; // ��� �����
    GLenum m_textureTarget; // ����� ������ �������� ��������
    GLuint m_textureObj; // ������ � ����� ����������� ��������: �������
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;

    //Magick::Image* m_pImage; // ��������� �� ����������� (��������� �� ����� ������ ������ � ������)
    //// ������� �������� ������ - ��� �������� �������� ��� ��������
    //// �������������� ����������� � ������ ���,
    //// ��� ��� ����� ���� ������������ ���������� �����������
    //Magick::Blob m_blob;
};
