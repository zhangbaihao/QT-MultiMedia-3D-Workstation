#include "Renderer.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

Renderer::Renderer(QWidget *parent) : QOpenGLWidget(parent)
    , m_program(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_isDragging(false)
{}

Renderer::~Renderer()
{
    makeCurrent();
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_program) glDeleteProgram(m_program);
    doneCurrent();
}

void Renderer::loadModel(const QString &path)
{
    m_vertices.clear();
    m_indices.clear();

    if (path.endsWith(".stl", Qt::CaseInsensitive))
    {
        m_modelLoader.loadSTL(path, m_vertices, m_indices);
    }
    else if (path.endsWith(".ply", Qt::CaseInsensitive))
    {
        m_modelLoader.loadPLY(path, m_vertices, m_indices);
    }
    qDebug() << "Loading model from:" << path;
    setupBuffers();
    update();
}

void Renderer::setCameraPosition(const QVector3D &position)
{
    m_camera.setPosition(position);
    update();
}

void Renderer::setCameraTarget(const QVector3D &target)
{
    m_camera.setTarget(target);
    update();
}

void Renderer::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // 注意：Qt 6中，VAO相关函数需要OpenGL 3.0以上支持
    // 这里暂时注释掉VAO相关代码，使用简单的VBO方式
    setupShaders();
    // setupBuffers();
}

void Renderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_camera.setPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
}

void Renderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_program && !m_vertices.isEmpty())
    {
        glUseProgram(m_program);

        // 设置MVP矩阵
        QMatrix4x4 model;
        QMatrix4x4 view = m_camera.viewMatrix();
        QMatrix4x4 projection = m_camera.projectionMatrix();
        QMatrix4x4 mvp = projection * view * model;

        GLint mvpLocation = glGetUniformLocation(m_program, "mvp");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.data());

        // 绘制模型（使用VBO直接绘制）
        if (m_vbo && m_ebo)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            
            // 设置顶点属性
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(0);
            
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);
            
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
            glEnableVertexAttribArray(2);
            
            glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
            
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
}

void Renderer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_lastMousePos = event->pos();
        m_isDragging = true;
    }
}

void Renderer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging)
    {
        QPoint delta = event->pos() - m_lastMousePos;
        m_camera.rotate(delta.x() * 0.1f, -delta.y() * 0.1f);
        m_lastMousePos = event->pos();
        update();
    }
}

void Renderer::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() * 0.01f;
    m_camera.zoom(delta);
    update();
}

void Renderer::setupShaders()
{
    // 顶点着色器
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 mvp;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;

        void main()
        {
            gl_Position = mvp * vec4(aPos, 1.0);
            FragPos = aPos;
            Normal = aNormal;
            TexCoord = aTexCoord;
        }
    )";

    // 片段着色器
    const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;

        void main()
        {
            vec3 lightPos = vec3(1.0, 1.0, 1.0);
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            vec3 objectColor = vec3(0.5, 0.5, 0.5);

            // 环境光
            float ambientStrength = 0.1;
            vec3 ambient = ambientStrength * lightColor;

            // 漫反射
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            // 镜面反射
            float specularStrength = 0.5;
            vec3 viewDir = normalize(-FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;

            vec3 result = (ambient + diffuse + specular) * objectColor;
            FragColor = vec4(result, 1.0);
        }
    )";

    // 创建着色器程序
    m_program = glCreateProgram();

    // 编译顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glAttachShader(m_program, vertexShader);

    // 编译片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glAttachShader(m_program, fragmentShader);

    // 链接程序
    glLinkProgram(m_program);

    // 清理
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::setupBuffers()
{
    if (m_vertices.isEmpty())
        return;

    makeCurrent();

    // 清理旧的缓冲区
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);

    // 创建VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    // 创建EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // 解绑缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    doneCurrent();
}
