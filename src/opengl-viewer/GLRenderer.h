#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>

class GLModel;

class GLRenderer
{
public:
    GLRenderer();
    ~GLRenderer();

    bool initialize();
    void render(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);
    void setRenderMode(int mode); // 0: Solid, 1: Wireframe, 2: Points, 3: Solid+Wireframe
    void setMaterialProperties(const QVector3D &color, float metalness, float roughness);
    void setLightProperties(const QVector3D &ambientColor, const QVector3D &directionalColor, const QVector3D &lightDirection);

private:
    bool loadShader(const QString &vertexShaderPath, const QString &fragmentShaderPath, QOpenGLShaderProgram *&program);
    void renderSolid(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);
    void renderWireframe(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);
    void renderPoints(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

    QOpenGLShaderProgram *m_solidProgram;
    QOpenGLShaderProgram *m_wireframeProgram;
    QOpenGLShaderProgram *m_pointsProgram;
    
    int m_renderMode;
    QVector3D m_materialColor;
    float m_metalness;
    float m_roughness;
    QVector3D m_ambientColor;
    QVector3D m_directionalColor;
    QVector3D m_lightDirection;
};

#endif // GLRENDERER_H