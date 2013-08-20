#include "StdAfx.h"

#include "imagebox.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"

namespace gui
{
	ImageBox::ImageBox(System& sys, const std::string& name) :
		base_window(sys, name),
		m_img(0),
		m_vformat(Stretch),
		m_hformat(Stretch)
	{
	}

	ImageBox::~ImageBox(void)
	{
	}

	void ImageBox::setImageset(const std::string& set)
	{
		m_imgset = m_system.getWindowManager().loadImageset(set);
		invalidate();
	}

	const std::string ImageBox::getImageset() const
	{
		if(m_imgset)
			return m_imgset->GetName();

		return std::string();
	}

	void ImageBox::setImage(const std::string& image)
	{
		if(m_imgset && !image.empty())
		{
			const Imageset& set = *m_imgset;
			m_img = set[image];
		}
		else
			m_img = 0;

		invalidate();
	}

	const std::string ImageBox::getImage() const
	{
		if(m_img)
			return m_img->GetName();

		return std::string();
	}

	void ImageBox::render(const Rect& finalRect, const Rect& finalClip)
	{
		Renderer& r = m_system.getRenderer();

		if (m_img)
		{
			r.draw(*m_img, finalRect, 1.f, finalClip,  m_backColor, m_hformat, m_vformat);
		}
	}

	void ImageBox::init(xml::node& node)
	{
		base_window::init(node);

		xml::node setting = node("Imageset");
		if(!setting.empty())
		{
			setImageset(setting.first_child().value());
		}

		setting = node("VertFormatting");
		if(!setting.empty())
		{
			m_vformat = StringToImageOps(setting.first_child().value());
		}
		
		setting = node("HorzFormatting");
		if(!setting.empty())
		{
			m_hformat = StringToImageOps(setting.first_child().value());
		}

		setting = node("Image");
		if(!setting.empty())
		{
			setImage(setting.first_child().value());
		}
	}

}