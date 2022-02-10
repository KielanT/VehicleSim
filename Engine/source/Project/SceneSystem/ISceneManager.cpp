#include "ppch.h"
#include "ISceneManager.h"
#include "CDirectX11SceneManager.h"


namespace Project
{
    ISceneManager* Project::NewSceneManager(IRenderer* renderer)
    {
        if (renderer->GetRenderType() == ERendererType::DirectX11) // Returns the DirectX 11 scene manager
        {
            return new CDirectX11SceneManager(renderer, renderer->GetWindowsProperties());
        }
        else
        {
            return nullptr;
        }
    }
}

