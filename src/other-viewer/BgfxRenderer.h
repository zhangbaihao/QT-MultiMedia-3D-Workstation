#pragma once

#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <string>

class BgfxRenderer {
public:
    BgfxRenderer();
    ~BgfxRenderer();

    void initialize(uint32_t width, uint32_t height);
    void shutdown();
    void resize(uint32_t width, uint32_t height);
    void render();

    void setModel(const std::string& modelPath);
    void setColor(float r, float g, float b);
    void setTransform(const bx::Vec3& position, const bx::Quaternion& rotation, float scale);

    bool isInitialized() const { return m_initialized; }

private:
    void createShaders();
    void destroyShaders();

    bool m_initialized = false;
    uint32_t m_width = 800;
    uint32_t m_height = 600;
    
    bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;
    
    bx::Vec3 m_position = { 0.0f, 0.0f, 0.0f };
    bx::Quaternion m_rotation = bx::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    float m_scale = 1.0f;
    
    float m_color[3] = { 0.0f, 0.7f, 0.8f };
};
