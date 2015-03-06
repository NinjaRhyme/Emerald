#include "EEScene.h"
#include "EECore.h"
#include "EESort.h"

//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	//EEScene
	//----------------------------------------------------------------------------------------------------
	EEScene::EEScene(const Rect_Float &_rect)
		:
		EEQuad2D(_rect)
	{

	}

	//----------------------------------------------------------------------------------------------------
	EEScene::EEScene(const Rect_Float &_rect, const EETexture& _tex)
		:
		EEQuad2D(_rect, _tex)
	{
		//controls

	}

	//----------------------------------------------------------------------------------------------------
	EEScene::EEScene(const Rect_Float &_rect, ID3D11ShaderResourceView *_tex)
		:
		EEQuad2D(_rect, _tex)
	{
		//controls

	}

	//----------------------------------------------------------------------------------------------------
	EEScene::EEScene(const EEScene &_scene)
		:
		EEQuad2D(_scene),
		m_objects(m_objects)
	{

	}

	//----------------------------------------------------------------------------------------------------
	EEScene::~EEScene()
	{

	}

	//----------------------------------------------------------------------------------------------------
	bool EEScene::Update()
	{
		if (!EEQuad2D::Update())
			return false;

		for (UINT i = 0; i < m_objects.size(); ++i)
		{
			m_objects[i]->Update();
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool EEScene::Render()
	{
		if (!EEQuad2D::Render())
			return false;

		for (UINT i = 0; i < m_objects.size(); ++i)
		{
			m_objects[i]->Render();
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void EEScene::AddObject(EEObject* _object)
	{
		_object->SetParent(this);
		//if (m_objects.size() == m_objects.capacity())
		//	m_objects.reserve(m_objects.size() + 1);
		//m_objects.push_back(_object);
		EEInsertSort(m_objects, _object, true);
	}

	//----------------------------------------------------------------------------------------------------
	void EEScene::RemoveObject(EEObject* _object)
	{
		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (*it = _object)
			{
				m_objects.erase(it);
				break;
			}
		}
	}
}