#pragma once
#include "Project/Core.h"
#include "Project/Interfaces/IState.h"

namespace Project
{
	// Enum class for setting the Sampler states
	enum class ESamplerState
	{
		PointSampler = 0,
		TrilinearSampler,
		Anisotropic4xSampler
	};

	// Enum class for setting the Blend states
	enum class EBlendState
	{
		NoBlendingState = 0,
		AdditiveBlendingState,
	};

	// Enum class for setting the Rasterizer states
	enum class ERasterizerState
	{
		CullBackState = 0,
		CullFrontState,
		CullNoneState,
	};

	// Enum class for setting the Depth Stencil states
	enum class EDepthStencilState
	{
		UseDepthBufferState = 0,
		DepthReadOnlyState,
		NoDepthBufferState,
	};


	class P_API CDirectX11States : public IState
	{
	public:
		~CDirectX11States();

		// Initialize the states 
		virtual bool InitStates(std::shared_ptr<IRenderer> renderer) override;

		// Release the states
		virtual void ReleaseStates() override;
		
		ID3D11SamplerState*		 GetSamplerState(ESamplerState ss); // Returns the Sampler state 
		ID3D11BlendState*		 GetBlendState(EBlendState bs); // Returns the Blend state 
		ID3D11RasterizerState*	 GetRasterizerState(ERasterizerState rs); // Returns the Rasterizer state 
		ID3D11DepthStencilState* GetDepthStencilState(EDepthStencilState dss); // Returns the Depth Stencil state 

	private:

		// Member variables
		ID3D11SamplerState* m_PointSampler = nullptr; // Variable for the Point sampler
		ID3D11SamplerState* m_TrilinearSampler = nullptr; // Variable for the Trilinear sampler
		ID3D11SamplerState* m_Anisotropic4xSampler = nullptr; // Variable for the Anisotropic4x sampler

		ID3D11BlendState* m_NoBlendingState = nullptr;  // Variable for the No Blending state
		ID3D11BlendState* m_AdditiveBlendingState = nullptr; // Variable for the Additive Blending state

		ID3D11RasterizerState* m_CullBackState = nullptr; // Variable for the Cull Back state
		ID3D11RasterizerState* m_CullFrontState = nullptr; // Variable for the Cull Front state
		ID3D11RasterizerState* m_CullNoneState = nullptr; // Variable for the Cull None state

		ID3D11DepthStencilState* m_UseDepthBufferState = nullptr; // Variable for the Use Depth Buffer state
		ID3D11DepthStencilState* m_DepthReadOnlyState = nullptr; // Variable for the Depth Read Only state
		ID3D11DepthStencilState* m_NoDepthBufferState = nullptr; // Variable for the No Depth Buffer state
	}; 
}

