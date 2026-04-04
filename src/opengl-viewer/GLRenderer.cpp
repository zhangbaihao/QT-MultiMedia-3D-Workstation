#include "GLRenderer.h"
#include "GLModel.h"
#include <QFile>
#include <QDebug>

GLRenderer::GLRenderer()
    : m_solidProgram(nullptr)
    , m_wireframeProgram(nullptr)
    , m_pointsProgram(nullptr)
    , m_renderMode(0)
    , m_materialColor(0.0f, 0.5f, 1.0f)
    , m_metalness(0.1f)
    , m_roughness(0.5f)
    , m_ambientColor(0.5f, 0.5f, 0.5f)
    , m_directionalColor(1.0f, 1.0f, 1.0f)
    , m_lightDirection(5.0f, 5.0f, 5.0f)
{
}

GLRenderer::~GLRenderer()
{
    delete m_solidProgram;
    delete m_wireframeProgram;
    delete m_pointsProgram;
}

bool GLRenderer::initialize()
{
    // 加载着色器
    if (!loadShader("shaders/solid.vert", "shaders/solid.frag", m_solidProgram)) {
        return false;
    }
    
    if (!loadShader("shaders/wireframe.vert", "shaders/wireframe.frag", m_wireframeProgram)) {
        return false;
    }
    
    if (!loadShader("shaders/point.vert", "shaders/point.frag", m_pointsProgram)) {
        return false;
    }
    
    return true;
}

bool GLRenderer::loadShader(const QString &vertexShaderPath, const QString &fragmentShaderPath, QOpenGLShaderProgram *&program)
{
    program = new QOpenGLShaderProgram();
    
    // 加载顶点着色器
    QFile vertexShaderFile(vertexShaderPath);
    if (!vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open vertex shader file:" << vertexShaderPath;
        delete program;
        program = nullptr;
        return false;
    }
    
    QString vertexShaderSource = vertexShaderFile.readAll();
    vertexShaderFile.close();
    
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qDebug() << "Failed to compile vertex shader:" << program->log();
        delete program;
        program = nullptr;
        return false;
    }
    
    // 加载片段着色器
    QFile fragmentShaderFile(fragmentShaderPath);
    if (!fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open fragment shader file:" << fragmentShaderPath;
        delete program;
        program = nullptr;
        return false;
    }
    
    QString fragmentShaderSource = fragmentShaderFile.readAll();
    fragmentShaderFile.close();
    
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qDebug() << "Failed to compile fragment shader:" << program->log();
        delete program;
        program = nullptr;
        return false;
    }
    
    if (!program->link()) {
        qDebug() << "Failed to link shader program:" << program->log();
        delete program;
        program = nullptr;
        return false;
    }
    
    return true;
}

void GLRenderer::render(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix)
{
    switch (m_renderMode) {
    case 0: // Solid
        renderSolid(model, modelMatrix, viewMatrix, projectionMatrix);
        break;
    case 1: // Wireframe
        renderWireframe(model, modelMatrix, viewMatrix, projectionMatrix);
        break;
    case 2: // Points
        renderPoints(model, modelMatrix, viewMatrix, projectionMatrix);
        break;
    case 3: // Solid + Wireframe
        renderSolid(model, modelMatrix, viewMatrix, projectionMatrix);
        renderWireframe(model, modelMatrix, viewMatrix, projectionMatrix);
        break;
    }
}

void GLRenderer::renderSolid(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix)
{
    m_solidProgram->bind();
    model->bind();
    
    // 设置 uniforms
    m_solidProgram->setUniformValue("model", modelMatrix);
    m_solidProgram->setUniformValue("view", viewMatrix);
    m_solidProgram->setUniformValue("projection", projectionMatrix);
    m_solidProgram->setUniformValue("materialColor", m_materialColor);
    m_solidProgram->setUniformValue("metalness", m_metalness);
    m_solidProgram->setUniformValue("roughness", m_roughness);
    m_solidProgram->setUniformValue("ambientColor", m_ambientColor);
    m_solidProgram->setUniformValue("directionalColor", m_directionalColor);
    m_solidProgram->setUniformValue("lightDirection", m_lightDirection);
    
    // 渲染
    glDrawElements(GL_TRIANGLES, model->faceCount() * 3, GL_UNSIGNED_INT, nullptr);
    
    model->release();
    m_solidProgram->release();
}

void GLRenderer::renderWireframe(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix)
{
    m_wireframeProgram->bind();
    model->bind();
    
    // 设置 uniforms
    m_wireframeProgram->setUniformValue("model", modelMatrix);
    m_wireframeProgram->setUniformValue("view", viewMatrix);
    m_wireframeProgram->setUniformValue("projection", projectionMatrix);
    m_wireframeProgram->setUniformValue("wireframeColor", QVector3D(1.0f, 1.0f, 1.0f));
    
    // 渲染线框
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, model->faceCount() * 3, GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    model->release();
    m_wireframeProgram->release();
}

void GLRenderer::renderPoints(GLModel *model, const QMatrix4x4 &modelMatrix, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix)
{
    m_pointsProgram->bind();
    model->bind();
    
    // 设置 uniforms
    m_pointsProgram->setUniformValue("model", modelMatrix);
    m_pointsProgram->setUniformValue("view", viewMatrix);
    m_pointsProgram->setUniformValue("projection", projectionMatrix);
    m_pointsProgram->setUniformValue("pointColor", m_materialColor);
    
    // 渲染点云
    glDrawElements(GL_POINTS, model->vertexCount(), GL_UNSIGNED_INT, nullptr);
    
    model->release();
    m_pointsProgram->release();
}

void GLRenderer::setRenderMode(int mode)
{
    m_renderMode = mode;
}

void GLRenderer::setMaterialProperties(const QVector3D &color, float metalness, float roughness)
{
    m_materialColor = color;
    m_metalness = metalness;
    m_roughness = roughness;
}

void GLRenderer::setLightProperties(const QVector3D &ambientColor, const QVector3D &directionalColor, const QVector3D &lightDirection)
{
    m_ambientColor = ambientColor;
    m_directionalColor = directionalColor;
    m_lightDirection = lightDirection;
}