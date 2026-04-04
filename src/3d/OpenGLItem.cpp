#include "OpenGLItem.h"
#include "ModelLoader.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <cmath>

OpenGLRenderer::OpenGLRenderer()
    : m_time(0.0f), m_program(nullptr), m_indicesCount(0)
{
    initializeOpenGLFunctions();
    initShaders();
    m_currentRotation = QQuaternion(); // 初始为单位四元数
}

OpenGLRenderer::~OpenGLRenderer()
{
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
    delete m_program;
}

void OpenGLRenderer::synchronize(QQuickFramebufferObject *item)
{
    auto glItem = static_cast<OpenGLItem *>(item);

    m_scaleFactor = glItem->scaleFactor();
    m_translationX = glItem->translationX();
    m_translationY = glItem->translationY();
    m_rotationX = glItem->rotationX();
    m_rotationY = -glItem->rotationY();

    if (m_offsetX != glItem->offsetX() || m_offsetY != glItem->offsetY())
    {
        m_offsetX = glItem->offsetX();
        m_offsetY = glItem->offsetY();
        QVector3D rotationAxis = QVector3D(m_rotationY, m_rotationX, 0.0).normalized();
        QVector2D diff = QVector2D(m_offsetX, m_offsetY);
        double angle = diff.length();
        m_currentRotation = QQuaternion::fromAxisAndAngle(rotationAxis, angle) * m_currentRotation;
    }

    if (glItem)
    {
        QString model = glItem->modelFilename();
        if (!model.isEmpty() && model != m_loadedModel)
        {
            loadMesh(model);
            m_loadedModel = model;
        }
    }
}

void OpenGLRenderer::render()
{
    m_time += 0.01f;

    // 检查着色器程序是否正确初始化
    if (!m_program || !m_program->isLinked()) {
        qDebug() << "Shader program not initialized or linked";
        return;
    }

    // 检查framebufferObject是否存在
    if (!framebufferObject()) {
        qDebug() << "Framebuffer object not created";
        return;
    }

    glViewport(0, 0, framebufferObject()->width(), framebufferObject()->height());
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_program->bind();

    QMatrix4x4 model;
    model.translate(m_translationX, m_translationY, 0.0);
    model.translate(center);
    model.rotate(m_currentRotation);
    model.translate(-center);
    model.scale(m_scaleFactor);

    QMatrix4x4 view;
    view.lookAt(QVector3D(0, 0, 5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    QMatrix4x4 projection;
    projection.perspective(45.0f, float(framebufferObject()->width()) / framebufferObject()->height(), 0.1f, 100.0f);

    m_program->setUniformValue("model", model);
    m_program->setUniformValue("view", view);
    m_program->setUniformValue("projection", projection);

    if (m_indicesCount > 0) {
        m_vao.bind();
        glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);
        m_vao.release();
    } else {
        // 绘制默认立方体
        float vertices[] = {
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,

             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,

            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f
        };

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDisableVertexAttribArray(0);
    }

    m_program->release();

    update(); // 请求下一帧重绘
}

QOpenGLFramebufferObject *OpenGLRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    return new QOpenGLFramebufferObject(size, format);
}

void OpenGLRenderer::initShaders()
{
    m_program = new QOpenGLShaderProgram;
    const char *vertexShaderSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    const char *fragmentShaderSrc = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        void main() {
            vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
            float diff = max(dot(normalize(Normal), lightDir), 0.0);
            vec3 diffuse = diff * vec3(0.5);
            vec3 ambient = vec3(0.2);
            vec3 color = ambient + diffuse;
            FragColor = vec4(color, 1.0);
        }
    )";

    bool success = m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSrc);
    if (!success) {
        qDebug() << "Failed to add vertex shader:" << m_program->log();
        return;
    }

    success = m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSrc);
    if (!success) {
        qDebug() << "Failed to add fragment shader:" << m_program->log();
        return;
    }

    success = m_program->link();
    if (!success) {
        qDebug() << "Failed to link shader program:" << m_program->log();
        return;
    }

    qDebug() << "Shader program initialized successfully";
}

void OpenGLRenderer::loadMesh(const QString &filePath)
{
    ModelLoader loader;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;

    bool result = false;
    if (filePath.endsWith(".stl", Qt::CaseInsensitive)) {
        result = loader.loadSTL(filePath, vertices, indices);
    } else if (filePath.endsWith(".ply", Qt::CaseInsensitive)) {
        result = loader.loadPLY(filePath, vertices, indices);
    }

    if (!result) {
        qWarning() << "加载模型失败:" << filePath;
        return;
    }

    m_indicesCount = static_cast<int>(indices.size());

    initializeBuffers(vertices, indices);

    // 计算模型中心点
    center = QVector3D(0, 0, 0);
    for (const auto &vertex : vertices) {
        center += vertex.position;
    }
    center /= vertices.size();
}

void OpenGLRenderer::initializeBuffers(const QVector<Vertex> &vertices, const QVector<unsigned int> &indices)
{
    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    m_ebo.create();
    m_ebo.bind();
    m_ebo.allocate(indices.data(), indices.size() * sizeof(unsigned int));

    int stride = sizeof(Vertex);
    m_program->bind();
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, stride);

    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, stride);

    m_program->enableAttributeArray(2);
    m_program->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoord), 2, stride);
    m_program->release();

    m_vao.release();
}

OpenGLItem::OpenGLItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent), m_modelColor(Qt::blue)
{
    setFlag(ItemHasContents, true);
}

QQuickFramebufferObject::Renderer *OpenGLItem::createRenderer() const
{
    return new OpenGLRenderer();
}

void OpenGLItem::setModelColor(const QColor &color)
{
    if (m_modelColor != color) {
        m_modelColor = color;
        emit modelColorChanged();
        update();
    }
}

void OpenGLItem::setScaleFactor(qreal value)
{
    if (m_scaleFactor != value) {
        m_scaleFactor = value;
        emit scaleFactorChanged();
        update();
    }
}

void OpenGLItem::setTranslationX(qreal value)
{
    if (m_translationX != value) {
        m_translationX = value;
        emit translationXChanged();
        update();
    }
}

void OpenGLItem::setTranslationY(qreal value)
{
    if (m_translationY != value) {
        m_translationY = value;
        emit translationYChanged();
        update();
    }
}

void OpenGLItem::updateRotation(qreal deltaX, qreal deltaY)
{
    m_rotationX = deltaX;
    m_rotationY = deltaY;
    m_offsetX = deltaX;
    m_offsetY = deltaY;
    update();
}

void OpenGLItem::loadModel(const QString &filename)
{
    m_modelFilename = filename;
    update(); // 通知 Qt 重新渲染
}

void OpenGLItem::resetView()
{
    m_rotationX = 0;
    m_rotationY = 0;
    m_offsetX = 0;
    m_offsetY = 0;
    m_scaleFactor = 1.0;
    m_translationX = 0;
    m_translationY = 0;
    emit scaleFactorChanged();
    emit translationXChanged();
    emit translationYChanged();
    update();
}

QString OpenGLItem::modelFilename() const
{
    return m_modelFilename;
}