#include "OpenGLRenderer.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <fstream>
#include <sstream>

OpenGLRenderer::OpenGLRenderer() {
}

OpenGLRenderer::~OpenGLRenderer() {
    shutdown();
}

void OpenGLRenderer::initialize() {
    if (m_initialized) {
        return;
    }

    initializeOpenGLFunctions();

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 启用背面剔除
    glEnable(GL_CULL_FACE);
    // 设置多边形模式为填充
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 创建着色器
    createShaders();

    // 创建默认的立方体模型
    float vertices[] = {
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    };

    GLuint indices[] = {
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,
        0, 2, 4,
        4, 2, 6,
        1, 5, 3,
        5, 7, 3,
        0, 4, 1,
        1, 4, 5,
        2, 3, 6,
        6, 3, 7,
    };

    for (int i = 0; i < 8; ++i) {
        OpenGLVertex v;
        v.x = vertices[i * 6];
        v.y = vertices[i * 6 + 1];
        v.z = vertices[i * 6 + 2];
        v.nx = vertices[i * 6 + 3];
        v.ny = vertices[i * 6 + 4];
        v.nz = vertices[i * 6 + 5];
        m_vertices.push_back(v);
    }

    for (int i = 0; i < 36; ++i) {
        m_indices.push_back(indices[i]);
    }

    createBuffers();
    m_initialized = true;
}

void OpenGLRenderer::shutdown() {
    if (!m_initialized) {
        return;
    }

    destroyBuffers();
    destroyShaders();
    m_initialized = false;
}

void OpenGLRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void OpenGLRenderer::render() {
    if (!m_initialized) {
        return;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);

    // 设置模型矩阵
    QMatrix4x4 model;
    model.translate(m_position);
    model.rotate(m_rotation);
    model.scale(m_scale);

    // 设置视图矩阵
    QMatrix4x4 view;
    view.lookAt(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    // 计算MVP矩阵
    QMatrix4x4 mvp = m_projection * view * model;

    // 设置 uniforms
    GLint mvpLoc = glGetUniformLocation(m_program, "u_mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.constData());

    GLint colorLoc = glGetUniformLocation(m_program, "u_color");
    glUniform3f(colorLoc, m_color.x(), m_color.y(), m_color.z());

    // 绘制
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void OpenGLRenderer::setModel(const std::string& modelPath) {
    // 这里可以实现模型加载逻辑
    std::cout << "Loading model: " << modelPath << std::endl;
}

void OpenGLRenderer::setColor(float r, float g, float b) {
    m_color = QVector3D(r, g, b);
}

void OpenGLRenderer::setTransform(const QVector3D& position, const QQuaternion& rotation, float scale) {
    m_position = position;
    m_rotation = rotation;
    m_scale = scale;
}

void OpenGLRenderer::createShaders() {
    // 顶点着色器
    const char* vsSource = R"(
        #version 330 core
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_normal;
        uniform mat4 u_mvp;
        void main() {
            gl_Position = u_mvp * vec4(a_position, 1.0);
        }
    )";

    // 片段着色器
    const char* fsSource = R"(
        #version 330 core
        uniform vec3 u_color;
        out vec4 fragColor;
        void main() {
            fragColor = vec4(u_color, 1.0);
        }
    )";

    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vsSource, nullptr);
    glCompileShader(m_vertexShader);

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fsSource, nullptr);
    glCompileShader(m_fragmentShader);

    m_program = glCreateProgram();
    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);
    glLinkProgram(m_program);
}

void OpenGLRenderer::destroyShaders() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    if (m_vertexShader) {
        glDeleteShader(m_vertexShader);
        m_vertexShader = 0;
    }

    if (m_fragmentShader) {
        glDeleteShader(m_fragmentShader);
        m_fragmentShader = 0;
    }
}

void OpenGLRenderer::createBuffers() {
    // 创建VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // 创建VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(OpenGLVertex), m_vertices.data(), GL_STATIC_DRAW);

    // 创建EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    // 设置顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OpenGLVertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OpenGLVertex), (void*)(3 * sizeof(float)));

    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLRenderer::destroyBuffers() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_ebo) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
}
