#include "ppch.h"
#include "IState.h"
#include "Graphics/DirectX11/CDirectX11States.h"


namespace Project
{
    std::unique_ptr<IState> SetStates(ERendererType type)
    {
        if (type == ERendererType::DirectX11) // Returns the DirectX 11 States
        {
            return std::make_unique<CDirectX11States>();
        }
        else
        {
            return nullptr;
        }
    }
}
