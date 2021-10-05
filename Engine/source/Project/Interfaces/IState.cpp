#include "ppch.h"
#include "IState.h"
#include "Graphics/DirectX11/CDirectX11States.h"


namespace Project
{
    IState* SetStates(ERendererType type)
    {
        if (type == ERendererType::DirectX11) // Returns the DirectX 11 States
        {
            return new CDirectX11States();
        }
        else
        {
            return nullptr;
        }
    }
}
