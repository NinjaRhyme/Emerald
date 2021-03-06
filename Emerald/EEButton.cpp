#include "EEButton.h"
#include "EECore.h"


//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	//EEButton
	//----------------------------------------------------------------------------------------------------
	EEButton::EEButton(const Rect_Float& _rect, const EETexture& _tex, std::function<void(void)> _funcPtr)
		:
		EEQuad2D(_rect, _tex),
		m_overTex(_tex),
		m_downTex(_tex),
		// callback function
		m_callbackFunc(_funcPtr)
	{
		SetIsFocusable(true);
	}

	//----------------------------------------------------------------------------------------------------
	EEButton::EEButton(const Rect_Float& _rect, const EETexture& _upTex, const EETexture& _overTex, const EETexture& _downTex, std::function<void(void)> _funcPtr)
		:
		EEQuad2D(_rect, _upTex),
		m_overTex(_overTex),
		m_downTex(_downTex),
		// callback function
		m_callbackFunc(_funcPtr)
	{
		SetIsFocusable(true);
	}

	//----------------------------------------------------------------------------------------------------
	EEButton::EEButton(const EEButton& _Button)
		:
		EEQuad2D(_Button),
		m_overTex(_Button.m_overTex),
		m_downTex(_Button.m_downTex),
		// callback function
		m_callbackFunc(_Button.m_callbackFunc)
	{
		SetIsFocusable(true);
	}

	//----------------------------------------------------------------------------------------------------
	EEButton::~EEButton()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool EEButton::Update()
	{
		if (!EEObject::Update())
			return false;

		UpdateObjectState();

		if (m_isPositionDirty)
		{
			m_isPositionDirty = false;
		}

		if (m_isScaleDirty || m_isLocalZOrderDirty)
		{
			Rect_Float rect(
				-m_quadWidth / 2,
				-m_quadHeight / 2,
				m_quadWidth / 2,
				m_quadHeight / 2
				);

			EEQuad2DVertex vertices[4];
			vertices[0].pos = FLOAT3(rect.x, rect.y, m_localZOrder * 0.0001f);
			vertices[0].tex = FLOAT2(0, 0);
			vertices[1].pos = FLOAT3(rect.z, rect.y, m_localZOrder * 0.0001f);
			vertices[1].tex = FLOAT2(1, 0);
			vertices[2].pos = FLOAT3(rect.x, rect.w, m_localZOrder * 0.0001f);
			vertices[2].tex = FLOAT2(0, 1);
			vertices[3].pos = FLOAT3(rect.z, rect.w, m_localZOrder * 0.0001f);
			vertices[3].tex = FLOAT2(1, 1);

			ID3D11DeviceContext *deviceContext = EECore::s_EECore->GetDeviceContext();
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			deviceContext->Map(m_quadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, vertices, sizeof(vertices));
			deviceContext->Unmap(m_quadVB, 0);

			m_isPositionDirty = false;
			m_isScaleDirty = false;
			m_isLocalZOrderDirty = false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool EEButton::Render()
	{
		if (!EEObject::Render())
			return false;

		MapObjectBuffer();

		ID3D11DeviceContext *deviceConstext = EECore::s_EECore->GetDeviceContext();
		deviceConstext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		deviceConstext->IASetInputLayout(s_quadIL);
		UINT stride = sizeof(EEQuad2DVertex);
		UINT offset = 0;
		deviceConstext->IASetVertexBuffers(0, 1, &m_quadVB, &stride, &offset);
		deviceConstext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		deviceConstext->VSSetShader(s_quadVS, NULL, 0);
		ID3D11ShaderResourceView *texture = NULL;
		switch (m_state)
		{
		case EE_OBJECT_FREE:
			texture = m_tex.GetTexture();
			deviceConstext->PSSetShaderResources(0, 1, &texture);
			break;
		case EE_OBJECT_DOWN:
			texture = m_downTex.GetTexture();
			deviceConstext->PSSetShaderResources(0, 1, &texture);
			break;
		case EE_OBJECT_OVER:
			texture = m_overTex.GetTexture();
			deviceConstext->PSSetShaderResources(0, 1, &texture);
			break;
		}
		deviceConstext->PSSetShader(s_quadPS, NULL, 0);
		deviceConstext->Draw(4, 0);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	Rect_Float EEButton::GetFinalRect() const
	{
		if (m_parent)
		{
			FLOAT3 scale = (m_parent->GetFinalScale() * m_scale - 1.0f) * 0.5f;
			FLOAT3 deltaPos = m_parent->GetFinalPosition();
			return Rect_Float(
				m_quadRect.x - m_quadWidth * scale.x + deltaPos.x,
				m_quadRect.y - m_quadHeight * scale.y + deltaPos.y,
				m_quadRect.z + m_quadWidth * scale.x + deltaPos.x,
				m_quadRect.w + m_quadHeight * scale.y + deltaPos.y
				);
		}
		else
		{
			FLOAT3 scale = (m_scale - 1.0f) * 0.5f;
			return Rect_Float(
				m_quadRect.x - m_quadWidth * scale.x,
				m_quadRect.y - m_quadHeight * scale.y,
				m_quadRect.z + m_quadWidth * scale.x,
				m_quadRect.w + m_quadHeight * scale.y
				);
		}

		return Rect_Float(0.0f, 0.0f, 0.0f, 0.0f);
	}

	//----------------------------------------------------------------------------------------------------
	EETexture* EEButton::GetUpTex()
	{
		return &m_tex;
	}

	//----------------------------------------------------------------------------------------------------
	EETexture* EEButton::GetOverTex()
	{
		return &m_overTex;
	}

	//----------------------------------------------------------------------------------------------------
	EETexture* EEButton::GetDownTex()
	{
		return &m_downTex;
	}

	//----------------------------------------------------------------------------------------------------
	void EEButton::OnMouseOver(const Point& _pos)
	{
		EEObject::OnMouseOver(_pos);
	}

	//----------------------------------------------------------------------------------------------------
	void EEButton::OnMouseClicked(const Point& _pos)
	{
		EEObject::OnMouseClicked(_pos);
	}

	 //----------------------------------------------------------------------------------------------------
	void EEButton::OnMouseTriggered(const Point& _pos)
	{
		EEObject::OnMouseTriggered(_pos);

		if (m_callbackFunc)
		{
			//(*(void(*)())m_callbackFunc)();
			m_callbackFunc();
			s_triggeredObject = nullptr;
		}
	}
}