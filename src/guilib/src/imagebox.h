#pragma once

#include "window.h"
#include "imagesetmanager.h"
#include "imageops.h"

namespace gui
{
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class ImageBox : public base_window
	{
	public:
		typedef ImageBox Self;
		ImageBox(System& sys, const std::string& name = std::string());
		virtual ~ImageBox();
		
		static const char* GetType() { return "ImageBox"; }
		virtual const char* getType() const { return Self::GetType(); }

		void setImageset(const std::string& set);
		const std::string getImageset() const;

		void setImage(const std::string& image);
		const std::string getImage() const;

		void setVertFormat(const std::string& format)
		{
			m_vformat = StringToImageOps(format);
			invalidate();
		}
		const std::string getVertFormat() const { return ImageOpsToString(m_vformat); }

		void setHorzFormat(const std::string& format)
		{
			m_hformat = StringToImageOps(format);
			invalidate();
		}
		const std::string getHorzFormat() const { return ImageOpsToString(m_hformat); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

	protected:
		ImagesetPtr m_imgset;
		const Image* m_img;
		ImageOps	m_vformat;
		ImageOps	m_hformat;
	};
}