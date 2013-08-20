#include "StdAfx.h"

#include "editbox.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "font.h"
#include "utils.h"

namespace gui
{
	Editbox::Editbox(System& sys, const std::string& name) :
		Label(sys, name),
		m_backImg(0),
		m_leftImg(0),
		m_rightImg(0),
		m_caretImg(0),
		m_selectImg(0),
		m_caretPos(0),
		m_selectionStart(0),
		m_selectionEnd(0),
		m_dragPos(0),
		m_selecting(false),
		m_maxLength(65535),
		m_editOffset(0.f),
		m_readOnly(false),
		m_maskChar(L'*'),
		m_password(false),
		m_numeric(false)
	{
	}

	Editbox::~Editbox(void)
	{
	}

	void Editbox::setText(const std::string& text)
	{
		invalidate();
		m_text = text;
		m_wtext = UTF8ToUTF16(text);
		setCaretPos(m_wtext.length());
	}

	const std::string Editbox::getText() const
	{
		return UTF16ToUTF8(m_wtext);
	}

	void Editbox::setNumeric(bool status) 
	{
		m_numeric = status;
		setText("");
		invalidate();
	}

	bool Editbox::onMouseMove(void)
	{
		if(m_selecting)
		{
			size_t end = getTextIndexAtPoint(m_system.getCursor().getPosition());
			setCaretPos(end);
			setSelection(m_dragPos, end);
			invalidate();
		}
		return true;
	}

	bool Editbox::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Down)
		{
			setInputFocus(true);
			m_system.queryCaptureInput(this);
			clearSelection();
			m_selecting = true;
			m_dragPos = getTextIndexAtPoint(m_system.getCursor().getPosition());
			setCaretPos(m_dragPos);
			invalidate();
		}
		else
		{
			if(m_selecting)
			{
				m_selecting = false;
				size_t end = getTextIndexAtPoint(m_system.getCursor().getPosition());
				setCaretPos(end);
				setSelection(m_dragPos, end);

				m_system.queryCaptureInput(0);
				invalidate();
			}		
		}
		return true;
	}

	bool Editbox::onCaptureLost(void)
	{
		if(m_selecting)
		{
			size_t end = getTextIndexAtPoint(m_system.getCursor().getPosition());
			setCaretPos(end);
			setSelection(m_dragPos, end);
		}
		invalidate();
		m_selecting = false;
		return true;
	}

	bool Editbox::onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Down)
		{
			switch(key)
			{
			case EventArgs::K_LEFT:
				if(m_caretPos > 0)
					setCaretPos(m_caretPos-1);
				break;
			case EventArgs::K_RIGHT:
				setCaretPos(m_caretPos+1);
				break;
			case EventArgs::K_HOME:
				setCaretPos(0);
				break;
			case EventArgs::K_END:
				setCaretPos(m_wtext.length());
				break;
			case EventArgs::K_DELETE:
				if (!m_readOnly)
					handleDelete();		
				break;
			case EventArgs::K_BACK:
				if (!m_readOnly)
					handleBackspace();
				break;
			default:
				return Label::onKeyboardButton(key, state);
			};

			clampCaret();
			clearSelection();
		}
		return Label::onKeyboardButton(key, state);
	}

	bool Editbox::ValidateNumeric(const std::wstring& text) const
	{
		if(text.empty())
			return true;

		static const wchar_t * mask = L"0123456789";
		return text.find_first_not_of(mask) == std::wstring::npos;
	}

	bool Editbox::onChar(const wchar_t* text)
	{
		if (m_readOnly)
			return true;

		if(m_numeric)
		{
			if(!ValidateNumeric(std::wstring(text)))
				return true;
		}

		size_t len = m_selectionEnd - m_selectionStart;
		if(len > 0)
		{
			m_wtext.erase(m_selectionStart, len);
			setCaretPos(m_selectionStart);
		}
		clearSelection();
		size_t newLen = m_wtext.length() + 1;
		if (newLen <= m_maxLength)
		{
			m_wtext.insert(m_caretPos, text);
			setCaretPos(m_caretPos+1);
		}
		invalidate();
		return Label::onChar(text);
	}

	bool Editbox::onSized(bool update)
	{
		Label::onSized();
		setCaretPos(m_caretPos);
		invalidate();
		return true;
	}

	void Editbox::clearSelection()
	{
		m_selectionStart = 0;
		m_selectionEnd = 0;
		invalidate();
	}

	void Editbox::setSelection(size_t start, size_t end)
	{
		m_selectionStart = start;
		m_selectionEnd = end;

		if(m_selectionStart > m_selectionEnd)
		{
			size_t tmp = m_selectionStart;
			m_selectionStart = m_selectionEnd;
			m_selectionEnd = tmp;
		}

		invalidate();
	}

	size_t Editbox::getTextIndexAtPoint(const point& pt)
	{
		size_t ret = 0;
		if(m_font)
		{
			point local = transformToWndCoord(pt);
			local -= m_area.getPosition();
			local.x -= m_editOffset;
			std::wstring outtext(m_wtext);
			if(m_password)
				outtext.assign(outtext.length(), m_maskChar);
			ret = m_font->getCharAtPixel(outtext, local.x);
		}
		return ret;
	}

	void Editbox::clampCaret()
	{
		if(m_caretPos > m_wtext.length())
			setCaretPos(m_wtext.length());
	}

	void Editbox::handleDelete()
	{
		size_t len = m_selectionEnd - m_selectionStart;
		if(len > 0)
		{
			m_wtext.erase(m_selectionStart, len);
			setCaretPos(m_selectionStart);
		}
		else
		{
			if(m_caretPos < m_wtext.length())
				m_wtext.erase(m_caretPos, 1);
		}
		invalidate();
		Label::onChar(L"");
	}
	void Editbox::handleBackspace()
	{
		size_t len = m_selectionEnd - m_selectionStart;
		if(len > 0)
		{
			m_wtext.erase(m_selectionStart, len);
			setCaretPos(m_selectionStart);
		}
		else
		{
			if(m_caretPos > 0)
			{
				setCaretPos(m_caretPos-1);
				m_wtext.erase(m_caretPos, 1);
			}
		}
		invalidate();
		Label::onChar(L"");
	}

	void Editbox::setCaretPos(size_t offset)
	{
		if(m_area.getWidth() <= 0.f)
			return;

		m_caretPos = offset;
		if(offset == 0)
		{
			m_editOffset = 0.f;
			return;
		}
		std::wstring outtext(m_wtext);
		if(m_password)
			outtext.assign(outtext.length(), m_maskChar);
		float cursor = m_font->getFormattedTextExtent(outtext.substr(0, offset), Rect(), m_format);
		float width = m_area.getWidth() - 6.f;
		float diff = cursor - width;
		if(diff > 0.f)
		{
			// if we out of widget rect
			// move text cursor to be visible
			if(m_editOffset > -diff)
			{
				m_editOffset = -diff;
			}
		}
		else
		{
			if(-m_editOffset > cursor)
				m_editOffset = -cursor;
		}
		invalidate();
	}

	void Editbox::setMaxLength(size_t l)
	{
		clearSelection();
		m_maxLength = l;
		if (m_caretPos > m_maxLength)
			m_caretPos = m_maxLength;
		size_t curLength = m_wtext.length();
		ptrdiff_t diff = curLength - m_maxLength;
		if (diff > 0)
			m_wtext.erase(curLength - diff, diff);
		invalidate();
	}

	void Editbox::render(const Rect& finalRect, const Rect& finalClip)
	{
		float left = 0;
		float right = 0;
		Rect componentRect;
		Size imgSize;

		Renderer& r = m_system.getRenderer();

		// left image
		if (m_leftImg)
		{
			// calculate final destination area
			imgSize = m_leftImg->GetSize();
			componentRect = finalRect;
			componentRect.m_right = componentRect.m_left + imgSize.width;
			left  = imgSize.width;

			// draw this element.
			r.draw(*m_leftImg, componentRect, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
		// right image
		if (m_rightImg)
		{
			imgSize = m_rightImg->GetSize();
			componentRect = finalRect;
			componentRect.m_left = finalRect.m_right - imgSize.width;
			right = imgSize.width;

			// draw this element.
			r.draw(*m_rightImg, componentRect, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
		// center image
		if (m_backImg)
		{
			componentRect = finalRect;
			componentRect.m_left += left;
			componentRect.m_right -= right;
	        
			// draw this element.
			r.draw(*m_backImg, componentRect, 1.f, finalClip,  m_backColor, Tile, Stretch);
		}

		if(!m_font) return;

		//TODO: draw separately selected and unselected text
		Rect dest(finalRect);
		dest.m_top += 1.f;
		dest.m_bottom -= 1.f;

		std::wstring outtext(m_wtext);
		if(m_password)
			outtext.assign(outtext.length(), m_maskChar);
		
		Rect clip(finalClip);
		
		dest.setWidth(m_font->getFormattedTextExtent(outtext, Rect(), m_format));
		dest.offset(point(m_editOffset, 0.f));
		dest.offset(point(4.f, 0.f));

		clip.setWidth(finalClip.getWidth() - 6.f);
		clip.offset(point(4.f, 0.f));
		clip.m_top += 1.f;
		clip.m_bottom -= 1.f;

		// selection
		if(m_selectionEnd != m_selectionStart)
		{
			float starts = 0.f;
			float stops = 0.f;
			if(m_selectionStart > 0)
			{
				std::wstring tmp = outtext.substr(0, m_selectionStart);
				starts = m_font->getFormattedTextExtent(tmp, Rect(), m_format);
			}
			std::wstring tmp1 = outtext.substr(0, m_selectionEnd);
			stops = m_font->getFormattedTextExtent(tmp1, Rect(), m_format);
			
			if(m_selectImg)
			{
				imgSize = m_selectImg->GetSize();
				
				Rect selrect(dest);
				selrect.m_left += starts;
				selrect.m_right = dest.m_left + stops;
				selrect.setHeight(imgSize.height);
				
				r.draw(*m_selectImg, selrect, 1.f, finalClip,  m_backColor, Tile, Stretch);
			}
		}

		{
			Rect rc(dest);
			float height = m_font->getLineSpacing();
			float offset = (rc.getHeight() - height) / 2;
			rc.offset(point(0.f, offset));

			m_font->drawText(outtext, rc, 1.0f, clip, m_format, m_foreColor, 1.f, 1.f);
		}	

		if(m_caretImg && m_focus && !m_readOnly)
		{
			float x = 0.f;
			if(m_caretPos)
			{
				std::wstring tmp = outtext.substr(0, m_caretPos);
				x = m_font->getFormattedTextExtent(tmp, Rect(), m_format);
			}
			imgSize = m_caretImg->GetSize();
			Rect caretrect(dest);
			caretrect.m_left += x;
			caretrect.setSize(imgSize);
			caretrect.offset(point(0.f, 2.f));
			r.draw(*m_caretImg, caretrect, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
	}

	void Editbox::init(xml::node& node)
	{
		Label::init(node);
		
		m_format = LeftAligned; // override static text settings (required)

		xml::node setting = node("ReadOnly");
		if(!setting.empty())
		{
			m_readOnly = StringToBool(setting.first_child().value());
		}
		setting = node("Password");
		if(!setting.empty())
		{
			m_password = StringToBool(setting.first_child().value());
		}
		setting = node("Numeric");
		if(!setting.empty())
		{
			m_numeric = StringToBool(setting.first_child().value());
		}
		setting = node("MaxLength");
		if(!setting.empty())
		{
			m_maxLength = (size_t)atoi(setting.first_child().value());
		}	
		setting = node("Text"); // override static text settings (required)
		if(!setting.empty())
		{
			setText(setting.first_child().value());
		}

		xml::node frame = node("FrameImagery");
		if(!frame.empty())
		{
			std::string setname = frame["Imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_backImg = set[frame("Background")["Image"].value()];
				m_leftImg = set[frame("Left")["Image"].value()];
				m_rightImg = set[frame("Right")["Image"].value()];
				m_caretImg = set[frame("Caret")["Image"].value()];
				m_selectImg = set[frame("Selection")["Image"].value()];
			}
		}
	}

	KeyBinder::KeyBinder(System& sys, const std::string& name) :
		Label(sys, name),
		m_backImg(0),
		m_leftImg(0),
		m_rightImg(0),
		m_active(false)
	{
	}

	KeyBinder::~KeyBinder(void)
	{
	}

	bool KeyBinder::onMouseMove(void)
	{
		return true;
	}

	bool KeyBinder::onMouseWheel(int delta)
	{
		if(m_active)
		{
			BinderEventArgs a;
			a.name = "On_KeyBind";
			a.isWheel = true;
			a.mwheel = delta;
			a.systemkey = m_system.GetSystemKeys();
			callHandler(&a);
			m_system.LeaveExclusiveInputMode();
			m_active = false;
		}
		return true;
	}

	bool KeyBinder::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(m_active)
		{
			if(state == EventArgs::Up)
			{
				BinderEventArgs a;
				a.name = "On_KeyBind";
				a.isMouse = true;
				a.mbutton = btn;
				a.systemkey = m_system.GetSystemKeys();
				callHandler(&a);
				m_system.LeaveExclusiveInputMode();
				m_active = false;
			}
		}
		else
		{
			if(btn == EventArgs::Left && state == EventArgs::Up)
			{
				// acquire input
				m_active = true;
				m_system.EnterExclusiveInputMode(this);
			}
		}
		return true;
	}

	bool KeyBinder::onMouseDouble(EventArgs::MouseButtons btn)
	{
		BinderEventArgs a;
		a.name = "On_KeyBind";
		a.isMouse = true;
		a.doubleClick = true;
		a.mbutton = btn;
		a.systemkey = m_system.GetSystemKeys();
		callHandler(&a);
		m_system.LeaveExclusiveInputMode();
		m_active = false;
		return true;
	}

	bool KeyBinder::onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state)
	{
		if(m_active)
		{
			if( key == EventArgs::K_LMENU ||
				key == EventArgs::K_LSHIFT ||
				key == EventArgs::K_LCONTROL)
				return true;

			if(state == EventArgs::Up)
			{
				BinderEventArgs a;
				a.name = "On_KeyBind";
				a.isKbd = true;
				a.kbutton = key;
				a.systemkey = m_system.GetSystemKeys();
				callHandler(&a);
				m_system.LeaveExclusiveInputMode();
				m_active = false;
			}
		}
		return true;
	}

	bool KeyBinder::onChar(const wchar_t* /*text*/)
	{
		return true;
	}

	void KeyBinder::render(const Rect& finalRect, const Rect& finalClip)
	{
		float left = 0;
		float right = 0;
		Rect componentRect;
		Size imgSize;

		Renderer& r = m_system.getRenderer();

		// left image
		if (m_leftImg)
		{
			// calculate final destination area
			imgSize = m_leftImg->GetSize();
			componentRect = finalRect;
			componentRect.m_right = componentRect.m_left + imgSize.width;
			left  = imgSize.width;

			// draw this element.
			r.draw(*m_leftImg, componentRect, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
		// right image
		if (m_rightImg)
		{
			imgSize = m_rightImg->GetSize();
			componentRect = finalRect;
			componentRect.m_left = finalRect.m_right - imgSize.width;
			right = imgSize.width;

			// draw this element.
			r.draw(*m_rightImg, componentRect, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
		// center image
		if (m_backImg)
		{
			componentRect = finalRect;
			componentRect.m_left += left;
			componentRect.m_right -= right;
	        
			// draw this element.
			r.draw(*m_backImg, componentRect, 1.f, finalClip,  m_backColor, Tile, Stretch);
		}

		if(m_font)
		{
			Rect dest(finalRect);
			dest.m_top += 1.f;
			dest.m_bottom -= 1.f;
			
			Rect clip(finalClip);
			
			dest.setWidth(m_font->getFormattedTextExtent(m_text, Rect(), m_format));
			dest.offset(point(4.f, 0.f));

			clip.setWidth(finalClip.getWidth() - 6.f);
			clip.offset(point(4.f, 0.f));
			clip.m_top += 1.f;
			clip.m_bottom -= 1.f;
			
			m_font->drawText(m_text, dest, 1.0f, clip, m_format, m_backColor, 1.f, 1.f);
		}
	}

	void KeyBinder::init(xml::node& node)
	{
		Label::init(node);
		
		m_format = LeftAligned; // override static text settings (required)
		m_tabstop = false;

		xml::node setting = node("Text"); // override static text settings (required)
		if(!setting.empty())
		{
			setText(setting.first_child().value());
		}

		xml::node frame = node("FrameImagery");
		if(!frame.empty())
		{
			std::string setname = frame["Imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_backImg = set[frame("Background")["Image"].value()];
				m_leftImg = set[frame("Left")["Image"].value()];
				m_rightImg = set[frame("Right")["Image"].value()];
			}
		}
	}
}
