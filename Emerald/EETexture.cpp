#include "EETexture.h"
#include "EECore.h"

//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	//EETexture
	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture()
		:
		EESmartPtr()
	{

	}

	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture(wchar_t* _file)
		:
		EESmartPtr()
	{
		ID3D11Device *device = EECore::s_EECore->GetDevice();
		ID3D11ShaderResourceView *texture = nullptr;
		D3DX11CreateShaderResourceViewFromFileW(device, _file, 0, 0, &texture, 0);

		m_value = new EETextureData(texture);

		
		ID3D11Resource *resource;
		D3DX11CreateTextureFromFileW(device, _file, 0, 0, &resource, 0);

		ID3D11DeviceContext* deviceContext = EECore::s_EECore->GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(deviceContext->Map(resource, 0, D3D11_MAP_READ, 0, &mappedResource)))
			return;
		(char*)mappedResource.pData;
		deviceContext->Unmap(resource, 0);
		
	}

	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture(const unsigned char* _buffer, unsigned int _width, unsigned int _height)
		:
		EESmartPtr()
	{
		ID3D11Device *device = EECore::s_EECore->GetDevice();

		D3D11_TEXTURE2D_DESC tex2DDesc;
		ZeroMemory(&tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tex2DDesc.Width = _width;
		tex2DDesc.Height = _height;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		//tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
		tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = _buffer;
		data.SysMemPitch = sizeof(unsigned int) * _width;
		data.SysMemSlicePitch = sizeof(unsigned int) * _width * _height;

		ID3D11Texture2D *texture2D = nullptr;
		HRESULT result;
		if (!FAILED(result = device->CreateTexture2D(&tex2DDesc, &data, &texture2D)))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			SRVDesc.Format = tex2DDesc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			ID3D11ShaderResourceView *texture = nullptr;
			if (!FAILED(device->CreateShaderResourceView(texture2D, &SRVDesc, &texture)))
			{
				m_value = new EETextureData(texture);
				SAFE_RELEASE(texture2D);
			}
		}
	}

	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture(EEBitmap& _bitmap)
		:
		EESmartPtr()
	{
		ID3D11Device *device = EECore::s_EECore->GetDevice();

		D3D11_TEXTURE2D_DESC tex2DDesc;
		ZeroMemory(&tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tex2DDesc.Width = _bitmap.GetWidth();
		tex2DDesc.Height = _bitmap.GetHeight();
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		//tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
		tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = _bitmap.GetData();
		data.SysMemPitch = sizeof(unsigned int)* _bitmap.GetWidth();
		data.SysMemSlicePitch = sizeof(unsigned int)* _bitmap.GetWidth() * _bitmap.GetHeight();

		ID3D11Texture2D *texture2D = nullptr;
		if (!FAILED(device->CreateTexture2D(&tex2DDesc, &data, &texture2D)))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			SRVDesc.Format = tex2DDesc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			ID3D11ShaderResourceView *texture = nullptr;
			if (!FAILED(device->CreateShaderResourceView(texture2D, &SRVDesc, &texture)))
			{
				m_value = new EETextureData(texture);
				SAFE_RELEASE(texture2D);
			}
		}
	}

	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture(ID3D11ShaderResourceView* _texture)
		:
		EESmartPtr()
	{
		m_value = new EETextureData(_texture);
	}

	//----------------------------------------------------------------------------------------------------
	EETexture::EETexture(const EETexture& _texture)
		:
		EESmartPtr(_texture)
	{

	}

	//----------------------------------------------------------------------------------------------------
	EETexture::~EETexture()
	{

	}

	//----------------------------------------------------------------------------------------------------
	bool EETexture::LoadTextureFromFile(LPCWSTR _file)
	{
		ID3D11ShaderResourceView *texture;
		if (FAILED(D3DX11CreateShaderResourceViewFromFileW(EECore::s_EECore->GetDevice(), _file, 0, 0, &texture, 0)))
			return false;
		SetValue(new EETextureData(texture));
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool EETexture::SetTexture(ID3D11ShaderResourceView *_texture)
	{
		SetValue(new EETextureData(_texture));

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	int EETexture::GetWidth()
	{
		if (m_value)
			return m_value->width;

		return 0;
	}

	//----------------------------------------------------------------------------------------------------
	int EETexture::GetHeight()
	{
		if (m_value)
			return m_value->height;

		return 0;
	}

	//----------------------------------------------------------------------------------------------------
	bool EETexture::GetBitmap(EEBitmap& _bitmap)
	{
		if (m_value)
		{
			if (m_value->texture)
			{
				// I spent a lot of time on it...
				ID3D11Resource *src = NULL;
				m_value->texture->GetResource(&src);
				D3D11_TEXTURE2D_DESC tex2DDesc;
				((ID3D11Texture2D *)src)->GetDesc(&tex2DDesc);
				tex2DDesc.BindFlags = 0;
				tex2DDesc.Usage = D3D11_USAGE_STAGING;
				tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

				ID3D11Texture2D *cpuBuf = nullptr;
				if (SUCCEEDED(EECore::s_EECore->GetDevice()->CreateTexture2D(&tex2DDesc, NULL, &cpuBuf)))
				{
					EECore::s_EECore->GetDeviceContext()->CopyResource(cpuBuf, src);

					ID3D11DeviceContext* deviceContext = EECore::s_EECore->GetDeviceContext();
					D3D11_MAPPED_SUBRESOURCE mappedResource;
					if (FAILED(deviceContext->Map(cpuBuf, 0, D3D11_MAP_READ, 0, &mappedResource)))
						return false;
					unsigned char* tmp = (unsigned char*)mappedResource.pData;
					_bitmap.SetData(tmp, m_value->width, m_value->height);
					deviceContext->Unmap(cpuBuf, 0);
					SAFE_RELEASE(cpuBuf);
					return true;
				}
			}
		}

		return false;
	}

	//----------------------------------------------------------------------------------------------------
	ID3D11ShaderResourceView* EETexture::GetTexture()
	{
		if (m_value)
			return m_value->texture;

		return nullptr;
	}

	//EETexture_APIs
	//----------------------------------------------------------------------------------------------------
	bool SaveTextureToFile(EETexture& _texture, LPCWSTR _fileName, EETextureType _type)
	{
		ID3D11DeviceContext *deviceContext = EECore::s_EECore->GetDeviceContext();
		ID3D11Resource *texture = NULL;
		_texture.GetTexture()->GetResource(&texture); // safety check
		switch (_type)
		{
		case EE_BMP:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_BMP, _fileName)))
				return false;
			break;
		case EE_JPG:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_JPG, _fileName)))
				return false;
			break;
		case EE_PNG:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_PNG, _fileName)))
				return false;
			break;
		case EE_DDS:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_DDS, _fileName)))
				return false;
			break;
		case EE_TIFF:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_TIFF, _fileName)))
				return false;
			break;
		case EE_GIF:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_GIF, _fileName)))
				return false;
			break;
		case EE_WMP:
			if (FAILED(D3DX11SaveTextureToFileW(deviceContext, texture, D3DX11_IFF_WMP, _fileName)))
				return false;
			break;
		default:
			return false;
		}

		return true;
	}
}