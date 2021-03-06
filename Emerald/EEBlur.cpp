#include "EEBlur.h"
#include "EECore.h"

#define GROUPDIX_N 256.f

//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	// EEBlurC
	//----------------------------------------------------------------------------------------------------
	bool EEBlurC::s_isBlurCInitialized = false;
	ID3D11Buffer *EEBlurC::s_blurBuffer = nullptr;
	ID3D11ComputeShader *EEBlurC::s_blurHorzCS = nullptr;
	ID3D11ComputeShader *EEBlurC::s_blurVertCS = nullptr;

	//----------------------------------------------------------------------------------------------------
	bool EEBlurC::InitializeBlurC()
	{
		if (!s_isBlurCInitialized)
		{
			HRESULT result;
			ID3D11Device* device = EECore::s_EECore->GetDevice();
			ID3D11DeviceContext* deviceContext = EECore::s_EECore->GetDeviceContext();

			// BlurBuffer
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.ByteWidth = sizeof(EEBlurBufferDesc);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			result = device->CreateBuffer(&bufferDesc, NULL, &s_blurBuffer);
			if (FAILED(result))
				return false;

			ID3D10Blob *errorMessage = nullptr;
			ID3D10Blob *computeShaderBuffer = nullptr;
			// CS
			result = D3DX11CompileFromFileW(L"EEShader\\EEBlurShader.hlsl", NULL, NULL, "BlurHorzCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &computeShaderBuffer, &errorMessage, NULL);
			if (FAILED(result))
			{
				if (errorMessage)
					MessageBoxA(NULL, (char*)errorMessage->GetBufferPointer(), "Compile Shader Error!", MB_OK);
				SAFE_RELEASE(errorMessage);
				SAFE_RELEASE(computeShaderBuffer);
				return false;
			}
			result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &s_blurHorzCS);
			if (FAILED(result))
			{
				SAFE_RELEASE(errorMessage);
				SAFE_RELEASE(computeShaderBuffer);
				return false;
			}
			result = D3DX11CompileFromFileW(L"EEShader\\EEBlurShader.hlsl", NULL, NULL, "BlurVertCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &computeShaderBuffer, &errorMessage, NULL);
			if (FAILED(result))
			{
				if (errorMessage)
					MessageBoxA(NULL, (char*)errorMessage->GetBufferPointer(), "Compile Shader Error!", MB_OK);
				SAFE_RELEASE(errorMessage);
				SAFE_RELEASE(computeShaderBuffer);
				return false;
			}
			result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &s_blurVertCS);
			if (FAILED(result))
			{
				SAFE_RELEASE(errorMessage);
				SAFE_RELEASE(computeShaderBuffer);
				return false;
			}

			s_isBlurCInitialized = true;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	EEBlurC::EEBlurC(EETexture& _target)
		:
		m_backup(),
		m_tmp(_target.GetWidth(), _target.GetHeight()),
		m_target(_target)
	{
		InitializeBlurC();

		if (m_target.GetTexture())
		{
			ID3D11Resource *resource = nullptr;
			m_target.GetTexture()->GetResource(&resource);
			D3D11_TEXTURE2D_DESC texture2DDesc;
			((ID3D11Texture2D*)resource)->GetDesc(&texture2DDesc);
			ID3D11Texture2D *resourceBackup = nullptr;
			if (SUCCEEDED(EECore::s_EECore->GetDevice()->CreateTexture2D(&texture2DDesc, NULL, &resourceBackup)))
			{
				m_backup.SetTexture(resourceBackup);
				EECore::s_EECore->GetDeviceContext()->CopyResource(resourceBackup, resource);
			}
		}
	}

	//----------------------------------------------------------------------------------------------------
	bool EEBlurC::Update()
	{
		if (!EEEffect::Update())
			return false;

		ID3D11DeviceContext* deviceContext = EECore::s_EECore->GetDeviceContext();

		// horz
		// set resource
		ID3D11ShaderResourceView *texture = m_backup.GetTexture();
		deviceContext->CSSetShaderResources(0, 1, &texture);
		ID3D11UnorderedAccessView *textureUAV = m_tmp.GetTextureUAV();
		deviceContext->CSSetUnorderedAccessViews(0, 1, &textureUAV, NULL);

		// dispatch
		deviceContext->CSSetShader(s_blurHorzCS, NULL, 0);
		deviceContext->Dispatch((int)ceilf(m_tmp.GetWidth() / GROUPDIX_N), m_tmp.GetHeight(), 1);

		// clear
		ID3D11ShaderResourceView *nullSRV[1] = { nullptr };
		deviceContext->CSSetShaderResources(0, 1, nullSRV);
		ID3D11UnorderedAccessView *nullUAV[1] = { nullptr };
		deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		// vert
		// set resource
		texture = m_tmp.GetTexture();
		deviceContext->CSSetShaderResources(0, 1, &texture);
		textureUAV = m_target.GetTextureUAV();
		deviceContext->CSSetUnorderedAccessViews(0, 1, &textureUAV, NULL);

		// dispatch
		deviceContext->CSSetShader(s_blurVertCS, NULL, 0);
		deviceContext->Dispatch(m_target.GetWidth(), (int)ceilf(m_tmp.GetHeight() / GROUPDIX_N), 1);

		// clear
		deviceContext->CSSetShaderResources(0, 1, nullSRV);
		deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool EEBlurC::Render()
	{
		if (!EEEffect::Render())
			return false;

		return true;
	}
}