#pragma once
#ifndef _EE_BLUR_H_
#define _EE_BLUR_H_

#include "EETexture.h"
#include "EEEffect.h"

//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	// EEBlurBufferDesc
	//----------------------------------------------------------------------------------------------------
	struct EEBlurBufferDesc
	{
		int value;
		float tmp1;
		float tmp2;
		float tmp3;

	};
	//EEBlurC (/Gaussian blur)
	//----------------------------------------------------------------------------------------------------
	class EEBlurC : public EEEffect
	{
	protected:
		static bool InitializeBlurC();

	protected:
		static bool s_isBlurCInitialized;
		static ID3D11Buffer *s_blurBuffer;
		static ID3D11ComputeShader *s_blurHorzCS;
		static ID3D11ComputeShader *s_blurVertCS;

	public:
		EEBlurC(EETexture& _target);

		virtual bool Update();
		virtual bool Render();

	protected:
		EETexture m_backup;
		EETexture m_tmp;
		EETexture m_target;
	};

}


#endif