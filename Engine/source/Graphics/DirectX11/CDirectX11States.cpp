#include "ppch.h"
#include "Graphics/DirectX11/CDirectX11States.h"
#include "Graphics/DirectX11/DirectX11Renderer.h"

namespace Project
{
    CDirectX11States::~CDirectX11States()
    {
        ReleaseStates();
    }

    bool CDirectX11States::InitStates(IRenderer* renderer)
    {
        //--------------------------------------------------------------------------------------
        // Texture Samplers
        //--------------------------------------------------------------------------------------
         // Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See texturing lab for details
        if (renderer->GetRenderType() == ERendererType::DirectX11)
        {
            DirectX11Renderer* dx11Renderer = static_cast<DirectX11Renderer*>(renderer);
            D3D11_SAMPLER_DESC samplerDesc = {};

            ////-------- Point Sampling (pixelated textures) --------////
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // Point filtering
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // Clamp addressing mode for texture coordinates outside 0->1
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;  // --"--
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;  // --"--
            samplerDesc.MaxAnisotropy = 1;                       // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
            samplerDesc.MinLOD = 0;                 // --"--

            // Then create a DirectX object for your description that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_PointSampler)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating point sampler");
                return false;
            }


            ////-------- Trilinear Sampling --------////
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Point filtering
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Wrap addressing mode for texture coordinates outside 0->1
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
            samplerDesc.MaxAnisotropy = 1;                       // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
            samplerDesc.MinLOD = 0;                 // --"--

            // Then create a DirectX object for your description that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_TrilinearSampler)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating Trilinear sampler");
                return false;
            }


            ////-------- Anisotropic filtering --------////
            samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Trilinear filtering
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap addressing mode for texture coordinates outside 0->1
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
            samplerDesc.MaxAnisotropy = 4;                        // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
            samplerDesc.MinLOD = 0;                 // --"--

            // Then create a DirectX object for your description that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_Anisotropic4xSampler)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating anisotropic 4x sampler");
                return false;
            }


            //--------------------------------------------------------------------------------------
            // Rasterizer States
            //--------------------------------------------------------------------------------------
            // Rasterizer states adjust how triangles are filled in and when they are shown
            // Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
            D3D11_RASTERIZER_DESC rasterizerDesc = {};

            ////-------- Back face culling --------////
            // This is the usual mode - don't show inside faces of objects
            rasterizerDesc.FillMode = D3D11_FILL_SOLID; // Can also set this to wireframe - experiment if you wish
            rasterizerDesc.CullMode = D3D11_CULL_BACK;  // Setting that decides whether the "front" and "back" side of each
                                                                     // triangle is drawn or not. Culling back faces is the norm
            rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_CullBackState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating cull-back state");
                return false;
            }


            ////-------- Front face culling --------////
            // This is an unusual mode - it shows inside faces only so the model looks inside-out
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Remove front faces
            rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_CullFrontState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating cull-front state");
                return false;
            }


            ////-------- No culling --------////
            // Used for transparent or flat objects - show both sides of faces
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_NONE;  // Don't cull any faces
            rasterizerDesc.DepthClipEnable = TRUE; // Advanced setting - only used in rare cases

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_CullNoneState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating cull-none state");
                return false;
            }


            //--------------------------------------------------------------------------------------
            // Blending States
            //--------------------------------------------------------------------------------------
            // Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See blending lab for details
            D3D11_BLEND_DESC blendDesc = {};

            ////-------- Blending Off State --------////
            blendDesc.RenderTarget[0].BlendEnable = FALSE;              // Disable blending
            blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;      // How to blend the source (texture colour)
            blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;     // How to blend the destination (colour already on screen)
            blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD

            //** Leave the following settings alone, they are used only in highly unusual cases
            //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
            blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            // Then create a DirectX object for the description that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateBlendState(&blendDesc, &m_NoBlendingState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating no-blend state");
                return false;
            }


            ////-------- Additive Blending State --------////

            blendDesc.RenderTarget[0].BlendEnable = TRUE;             // Enable blending
            blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;    // How to blend the source (texture colour)
            blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;    // How to blend the destination (colour already on screen)
            blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // How to combine the above two, almost always ADD

            //** Leave the following settings alone, they are used only in highly unusual cases
            //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
            blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            // Then create a DirectX object for the description that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateBlendState(&blendDesc, &m_AdditiveBlendingState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating additive blending state");
                return false;
            }


            //--------------------------------------------------------------------------------------
            // Depth-Stencil States
            //--------------------------------------------------------------------------------------
            // Depth-stencil states adjust how the depth and stencil buffers are used. The stencil buffer is rarely used so 
            // these states are most often used to switch the depth buffer on and off. See depth buffers lab for details
            // Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

            ////-------- Enable depth buffer --------////
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilDesc.StencilEnable = FALSE;

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_UseDepthBufferState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating use-depth-buffer state");
                return false;
            }


            ////-------- Enable depth buffer reads only --------////
            // Disables writing to depth buffer - used for transparent objects because they should not be entered in the buffer but do need to check if they are behind something
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable writing to depth buffer
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilDesc.StencilEnable = FALSE;

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_DepthReadOnlyState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating depth-read-only state");
                return false;
            }


            ////-------- Disable depth buffer --------////
            depthStencilDesc.DepthEnable = FALSE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilDesc.StencilEnable = FALSE;

            // Create a DirectX object for the description above that can be used by a shader
            if (FAILED(dx11Renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_NoDepthBufferState)))
            {
                ErrorLogger errorLog;
                errorLog.ErrorMessage(renderer->GetWindowsPropties(), "Error creating no-depth-buffer state");
                return false;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    void CDirectX11States::ReleaseStates()
    {
        if (m_PointSampler)         m_PointSampler->Release();
        if (m_TrilinearSampler)     m_TrilinearSampler->Release();
        if (m_Anisotropic4xSampler) m_Anisotropic4xSampler->Release();
        
        if (m_NoBlendingState)       m_NoBlendingState->Release();
        if (m_AdditiveBlendingState) m_AdditiveBlendingState->Release();
        
        if (m_CullBackState)  m_CullBackState->Release();
        if (m_CullFrontState) m_CullFrontState->Release();
        if (m_CullNoneState)  m_CullNoneState->Release();
        
        if (m_UseDepthBufferState) m_UseDepthBufferState->Release();
        if (m_DepthReadOnlyState)  m_DepthReadOnlyState->Release();
        if (m_NoDepthBufferState)  m_NoDepthBufferState->Release();
    }

    ID3D11SamplerState* CDirectX11States::GetSamplerState(ESamplerState ss)
    { 
        // Returns the selected sampler state
        switch (ss)
        {
        case ESamplerState::Anisotropic4xSampler:
            return m_Anisotropic4xSampler;
            break;
        case ESamplerState::PointSampler:
            return m_PointSampler;
            break;
        case ESamplerState::TrilinearSampler:
            return m_TrilinearSampler;
            break;
        }
    }

    ID3D11BlendState* CDirectX11States::GetBlendState(EBlendState bs)
    {
        // Returns the selected blending state
        switch (bs)
        {
        case EBlendState::NoBlendingState:
            return m_NoBlendingState;
            break;
        case EBlendState::AdditiveBlendingState:
            return m_AdditiveBlendingState;
            break;
        }

    }

    ID3D11RasterizerState* CDirectX11States::GetRasterizerState(ERasterizerState rs)
    {
        // Returns the selected Rasterizer state
        switch (rs)
        {
        case ERasterizerState::CullBackState:
            return m_CullBackState;
            break;
        case ERasterizerState::CullFrontState:
            return m_CullFrontState;
            break;
        case ERasterizerState::CullNoneState:
            return m_CullNoneState;
            break;
        }
    }

    ID3D11DepthStencilState* CDirectX11States::GetDepthStencilState(EDepthStencilState dss)
    {
        // Returns the selected Depth Stencil state
        switch (dss)
        {
        case EDepthStencilState::UseDepthBufferState:
            return m_UseDepthBufferState;
            break;
        case EDepthStencilState::DepthReadOnlyState:
            return m_DepthReadOnlyState;
            break;
        case EDepthStencilState::NoDepthBufferState:
            return m_NoDepthBufferState;
            break;
        }
    }
}
