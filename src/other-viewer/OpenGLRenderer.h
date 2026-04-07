#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
#include <string>
#include <vector>

struct OpenGLVertex {
    float x, y, z;
    float nx, ny, nz;
};

class OpenGLRenderer : public QOpenGLFunctions_3_3_Core {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void initialize();
    void shutdown();
    void resize(int width, int height);
    void render();

    void setModel(const std::string& modelPath);
    void setColor(float r, float g, float b);
    void setTransform(const QVector3D& position, const QQuaternion& rotation, float scale);
    bool isInitialized() const { return m_initialized; }

private:
    void createShaders();
    void destroyShaders();
    void createBuffers();
    void destroyBuffers();

    bool m_initialized = false;
    int m_width = 800;
    int m_height = 600;
    
    GLuint m_program = 0;
    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    
    QVector3D m_position = QVector3D(0.0f, 0.0f, 0.0f);
    QQuaternion m_rotation = QQuaternion();
    float m_scale = 1.0f;
    
    QVector3D m_color = QVector3D(0.0f, 0.7f, 0.8f);
    
    std::vector<OpenGLVertex> m_vertices;
    std::vector<GLuint> m_indices;
    
    QMatrix4x4 m_projection;
};
