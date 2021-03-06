#pragma once
#ifndef _EE_BUTTON_H_
#define _EE_BUTTON_H_

#include "EEQuad2D.h"
//#include <boost/any.hpp>


//----------------------------------------------------------------------------------------------------
namespace Emerald
{
	// EEButton
	//----------------------------------------------------------------------------------------------------
	class EEButton : public EEQuad2D
	{
	public:
		EEButton(const Rect_Float& _rect, const EETexture& _tex, std::function<void(void)> _funcPtr = [] {});
		EEButton(const Rect_Float& _rect, const EETexture& _upTex, const EETexture& _overTex, const EETexture& _downTex, std::function<void(void)> _funcPtr = []{});
		EEButton(const EEButton& _button);
		virtual ~EEButton();

		virtual bool Update();
		virtual bool Render();

		virtual Rect_Float GetFinalRect() const;
		EETexture* GetUpTex();
		EETexture* GetOverTex();
		EETexture* GetDownTex();

		// OnFunction
		virtual void OnMouseOver(const Point& _pos);
		virtual void OnMouseClicked(const Point& _pos);
		virtual void OnMouseTriggered(const Point& _pos);

	protected:
		EETexture m_overTex, m_downTex;
		// callback function
		std::function<void(void)> m_callbackFunc;
	};
}

#endif