#include "systems/Graphics.h"

namespace Systems {

    Graphics::Graphics() {}

    bool Graphics::Setup()
    {
        if (m_Initialized)
            return true;

        m_Initialized = true;
        spdlog::info("Graphics initialized successfully");
        return true;
    }

    bool Graphics::Shutdown()
    {
        m_RenderTargetView.Reset();
        m_BackBuffer.Reset();
        m_Context.Reset();
        m_Device.Reset();
        m_Initialized = false;
        return true;
    }

    void Graphics::CreateRenderTarget()
    {
    }

    void Graphics::ClearRenderTarget()
    {
    }
}
