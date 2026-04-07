#include "BgfxRenderer.h"
#include <bx/platform.h>
#include <iostream>

BgfxRenderer::BgfxRenderer() {
}

BgfxRenderer::~BgfxRenderer() {
    shutdown();
}

void BgfxRenderer::initialize(uint32_t width, uint32_t height) {
    if (m_initialized) {
        return;
    }

    m_width = width;
    m_height = height;

    // 初始化bgfx
    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.deviceId = 0;
    init.platformData.nwh = nullptr;
    init.platformData.ndt = nullptr;
    init.platformData.context = nullptr;
    init.platformData.backBuffer = nullptr;
    init.platformData.backBufferDS = nullptr;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize bgfx" << std::endl;
        return;
    }

    createShaders();
    m_initialized = true;
}

void BgfxRenderer::shutdown() {
    if (!m_initialized) {
        return;
    }

    destroyShaders();
    bgfx::shutdown();
    m_initialized = false;
}

void BgfxRenderer::resize(uint32_t width, uint32_t height) {
    if (!m_initialized) {
        return;
    }

    m_width = width;
    m_height = height;
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
}

void BgfxRenderer::render() {
    if (!m_initialized) {
        return;
    }

    // 开始帧
    bgfx::setViewRect(0, 0, 0, m_width, m_height);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1e1e1eff, 1.0f, 0);
    bgfx::touch(0);

    // 设置变换矩阵
    float view[16];
    bx::mtxLookAt(view, bx::Vec3(0.0f, 0.0f, -5.0f), bx::Vec3(0.0f, 0.0f, 0.0f), bx::Vec3(0.0f, 1.0f, 0.0f));

    float proj[16];
    bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    // 设置模型变换
    float model[16];
    float rotMat[16];
    bx::mtxFromQuaternion(rotMat, m_rotation);
    bx::mtxScale(model, m_scale, m_scale, m_scale);
    bx::mtxMul(model, model, rotMat);
    bx::mtxTranslate(model, m_position.x, m_position.y, m_position.z);

    // 简单的立方体模型
    static const float vertices[] = {
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
    };

    static const uint16_t indices[] = {
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

    // 创建顶点缓冲区
    if (m_vbh == BGFX_INVALID_HANDLE) {
        m_vbh = bgfx::createVertexBuffer(
            bgfx::makeRef(vertices, sizeof(vertices)),
            bgfx::VertexLayout().begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .end()
        );
    }

    // 创建索引缓冲区
    if (m_ibh == BGFX_INVALID_HANDLE) {
        m_ibh = bgfx::createIndexBuffer(
            bgfx::makeRef(indices, sizeof(indices))
        );
    }

    // 设置着色器和缓冲区
    bgfx::setTransform(model);
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setUniform(bgfx::createUniform("u_color", bgfx::UniformType::Vec3), m_color);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, m_program);

    // 结束帧
    bgfx::frame();
}

void BgfxRenderer::setModel(const std::string& modelPath) {
    // 这里可以实现模型加载逻辑
    std::cout << "Loading model: " << modelPath << std::endl;
}

void BgfxRenderer::setColor(float r, float g, float b) {
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
}

void BgfxRenderer::setTransform(const bx::Vec3& position, const bx::Quaternion& rotation, float scale) {
    m_position = position;
    m_rotation = rotation;
    m_scale = scale;
}

void BgfxRenderer::createShaders() {
    // 简单的顶点着色器
    static const char* vs = R"(
        #version 330
        uniform mat4 u_modelViewProj;
        in vec3 a_position;
        void main() {
            gl_Position = u_modelViewProj * vec4(a_position, 1.0);
        }
    )";

    // 简单的片段着色器
    static const char* fs = R"(
        #version 330
        uniform vec3 u_color;
        out vec4 fragColor;
        void main() {
            fragColor = vec4(u_color, 1.0);
        }
    )";

    bgfx::ShaderHandle vsh = bgfx::createShader(bgfx::makeRef(vs, strlen(vs)));
    bgfx::ShaderHandle fsh = bgfx::createShader(bgfx::makeRef(fs, strlen(fs)));
    m_program = bgfx::createProgram(vsh, fsh, true);
}

void BgfxRenderer::destroyShaders() {
    if (bgfx::isValid(m_program)) {
        bgfx::destroy(m_program);
        m_program = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(m_vbh)) {
        bgfx::destroy(m_vbh);
        m_vbh = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(m_ibh)) {
        bgfx::destroy(m_ibh);
        m_ibh = BGFX_INVALID_HANDLE;
    }
}
