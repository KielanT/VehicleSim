#include "ppch.h"
#include "ISceneManager.h"
#include "CDirectX11SceneManager.h"


namespace Project
{
    std::shared_ptr<ISceneManager> Project::NewSceneManager(std::shared_ptr<IRenderer> renderer)
    {
        if (renderer->GetRenderType() == ERendererType::DirectX11) // Returns the DirectX 11 scene manager
        {
            return std::make_shared<CDirectX11SceneManager>(renderer, renderer->GetWindowsProperties());
        }
        else
        {
            return nullptr;
        }
    }
}

