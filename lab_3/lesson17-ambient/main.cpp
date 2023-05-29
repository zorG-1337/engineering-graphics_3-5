#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

struct Vertex
{
    Vector3f m_pos; // позиция
    Vector2f m_tex; // координата текстуры в вершине

    Vertex() {}

    Vertex(Vector3f pos, Vector2f tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

class Main : public ICallbacks
{
public:

    Main()
    {
        m_pGameCamera = NULL;
        m_pTexture = NULL;
        m_pEffect = NULL;
        m_scale = 0.0f;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.5f;
    }

    ~Main()
    {
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pTexture;
    }

    bool Init()
    {
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT); // инициализация камеры 

        CreateVertexBuffer(); // создание массива вершин
        CreateIndexBuffer(); // создание индексного массива

        m_pEffect = new LightingTechnique(); // инициализация светового эффекта

        if (!m_pEffect->Init())
        {
            return false;
        }

        m_pEffect->Enable(); // включение светового эффекта

        m_pEffect->SetTextureUnit(0);  // передача в шейдер модуля текстуры (1)

        m_pTexture = new Texture(GL_TEXTURE_2D, "test.png"); // инициализация текстуры

        if (!m_pTexture->Load()) { // если не удалось
            return false;
        }

        return true;
    }

    void Run() // запуск инициализации
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB() // функция отрисовки
    {
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT);

        m_scale += 0.1f;

        Pipeline p;
        p.Rotate(0.0f, m_scale, 0.0f); // поворот объекта вокруг оси
        p.WorldPos(0.0f, 0.0f, 5.0f); // положение объекта
        // установка векторов камеры
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        // проекция перспективы
        p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
        m_pEffect->SetWVP(p.GetTrans()); // передача матрицы трасформации в шейдер
        m_pEffect->SetDirectionalLight(m_directionalLight);

        glEnableVertexAttribArray(0); // разрешение использования атрибутов вершины (0 - координата, 1 - текстура)
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);  // привязка вершинного буфера 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // параметры аттрибутов вершин
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO); // призязка индексного буфера
        m_pTexture->Bind(GL_TEXTURE0);  // привязываем текстуру к конкретному модулю GL_TEXTURE
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);  // функция отрисовки элементов (прямоугольники)

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glutSwapBuffers();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
        case 'q': // закрытие программы
            glutLeaveMainLoop();
            break;

        case 'a': // увеличение интенсивности фонового света
            m_directionalLight.AmbientIntensity += 0.05f;
            break;

        case 's': // уменьшение интенсивности фонового света
            m_directionalLight.AmbientIntensity -= 0.05f;
            break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

private:

    void CreateVertexBuffer() // вершинный буфер
    {
        Vertex Vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f)),
                               Vertex(Vector3f(0.0f, -1.0f, -1.15475), Vector2f(0.5f, 0.0f)),
                               Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.0f, 0.0f)),
                               Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.5f, 1.0f)) };

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void CreateIndexBuffer() // индексный буфер
    {
        unsigned int Indices[] = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   1, 2, 0 };

        glGenBuffers(1, &m_IBO); // генерация буфера
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO); // привязка буфера в качестве индексного
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(Indices), Indices, GL_STATIC_DRAW); //привязка данных буфера

    }

    GLuint m_VBO;
    GLuint m_IBO;
    LightingTechnique* m_pEffect;
    Texture* m_pTexture;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionLight m_directionalLight;
};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv); // инициализация glut

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "OpenGL tutors")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}