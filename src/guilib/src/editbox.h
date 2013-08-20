#pragma once
#include "label.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef boost::shared_ptr<Imageset> ImagesetPtr;

	class  Editbox : public Label
	{
	public:
		typedef Editbox Self;
		Editbox(System& sys, const std::string& name = std::string());
		virtual ~Editbox();

		static const char* GetType() { return "Editbox"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void setText(const std::string& text);
		virtual const std::string getText() const;

		void setMaxLength(size_t l);
		size_t getMaxLength() const { return m_maxLength; };
		
		void setPassword(bool passw) 
		{
			m_password = passw;
			invalidate();
		}
		bool getPassword() const { return m_password; }

		virtual bool onMouseMove();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state);
		virtual bool onChar(const wchar_t* text);
		virtual bool onCaptureLost();
		virtual bool onSized(bool update = true);

		void clearSelection();
		void setSelection(size_t start, size_t end);
		void setCaretPos(size_t offset);

		bool isNumeric() const { return m_numeric; }
		void setNumeric(bool status);

		bool ValidateNumeric(const std::wstring& text) const;

		virtual void init(xml::node& node);

	protected:
		size_t	getTextIndexAtPoint(const point& pt);	
		void	clampCaret();
		void	handleDelete();
		void	handleBackspace();

	protected:
		ImagesetPtr m_imgset;
		const Image*	m_backImg;
		const Image*	m_leftImg;
		const Image*	m_rightImg;
		const Image*	m_caretImg;
		const Image*	m_selectImg;

		size_t	m_caretPos;
		size_t	m_selectionStart;
		size_t	m_selectionEnd;
		size_t	m_dragPos;
		bool	m_selecting;
		size_t	m_maxLength;
		float	m_editOffset;
		std::wstring m_wtext;
		bool	m_readOnly;
		wchar_t m_maskChar;
		bool	m_password;
		bool	m_numeric;
	};

	class  KeyBinder : public Label
	{
	public:
		typedef KeyBinder Self;
		KeyBinder(System& sys, const std::string& name = std::string());
		virtual ~KeyBinder();

		static const char* GetType() { return "KeyBinder"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		
		virtual bool onMouseMove();
		virtual bool onMouseWheel(int delta);
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseDouble(EventArgs::MouseButtons btn);
		virtual bool onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state);
		virtual bool onChar(const wchar_t* text);

		virtual void init(xml::node& node);


	protected:
		ImagesetPtr		m_imgset;
		const Image*	m_backImg;
		const Image*	m_leftImg;
		const Image*	m_rightImg;

		bool			m_active;
	};
}