#include "StdAfx.h"

#include "progress.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "utils.h"

namespace gui
{

Progress::Progress(System& sys, const std::string& name) :
	Label(sys, name),
	m_progress(0)
{
}

Progress::~Progress(void)
{
}

void Progress::render(const Rect& finalRect, const Rect& finalClip)
{
	if(!m_text.empty())
		Label::render(finalRect, finalClip);
}

void Progress::init(xml::node& node)
{
	Label::init(node);

	xml::node setting = node("Progress");
	if(!setting.empty())
	{
		m_progress = (float)atof(setting.first_child().value());
	}
}


ProgressBar::ProgressBar(System& sys, const std::string& name) :
	Progress(sys, name),
	m_leftImg(0),
	m_rightImg(0),
	m_backImg(0),
	m_signed(false)
{
}

ProgressBar::~ProgressBar(void)
{
}

void ProgressBar::render(const Rect& finalRect, const Rect& finalClip)
{
	Renderer& r = m_system.getRenderer();

	// rendering caption
	float left = 0;
	float right = 0;
	float height = 0;
	
	Size imgSize;
	Rect targetRect(finalClip);
	if(m_signed)
	{
		float offset = targetRect.getWidth() / 2.f;
		targetRect.setWidth(offset);
		if(m_progress > 0.f)
			targetRect.offset(point(offset, 0.f));
	}
	Rect clip(targetRect);

	if(fabs(m_progress) > 1.f)
	{
		m_progress > 0.f ? m_progress = 1.f : m_progress = -1.f;
	}

	float clipsize = clip.getWidth() * fabs(m_progress);
	clip.setWidth(clipsize);
	
	if(m_progress < 0.f)
	{
		clip.m_left = targetRect.m_right - clipsize;
		clip.m_right = targetRect.m_right;
	}	

	Rect componentRect(targetRect);

	if (m_leftImg)
    {
        // calculate final destination area
		imgSize = m_leftImg->size();
		componentRect.m_right = componentRect.m_left + imgSize.width;
		componentRect.m_bottom = componentRect.m_top + imgSize.height;
		left  = imgSize.width;
		height = imgSize.height;

        // draw this element.
		r.draw(*m_leftImg, componentRect, 1.f, clip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }
	componentRect = targetRect;
	// right image
    if (m_rightImg)
    {
		imgSize = m_rightImg->size();
        componentRect.m_left = componentRect.m_right - imgSize.width;
        componentRect.m_bottom = componentRect.m_top + imgSize.height;

		right = imgSize.width;

        // draw this element.
		r.draw(*m_rightImg, componentRect, 1.f, clip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }
	componentRect = targetRect;
	// center image
    if (m_backImg)
    {
		componentRect.m_left += left;
		componentRect.m_right -= right;
		componentRect.m_bottom = componentRect.m_top + height;
        
		// draw this element.
		r.draw(*m_backImg, componentRect, 1.f, clip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
    }

	Progress::render(finalRect, finalClip);
}

void ProgressBar::init(xml::node& node)
{
	Progress::init(node);
	xml::node setting = node("Signed");
	if(!setting.empty())
	{
		m_signed = StringToBool(setting.first_child().value());
	}
	xml::node frame = node("Bar");
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

ImageBar::ImageBar(System& sys, const std::string& name) :
	Progress(sys, name),
	m_backImg(0),
	m_foreImg(0)
{
}

ImageBar::~ImageBar(void)
{
}

void ImageBar::render(const Rect& finalRect, const Rect& finalClip)
{
	Renderer& r = m_system.getRenderer();

	if (m_backImg)
    {		        
		// draw this element.
		r.draw(*m_backImg, finalRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }

	if (m_backImg)
    {
		Rect clip(finalClip);
		if(m_progress < 1.f)
		{
			clip.setWidth(clip.getWidth() * m_progress);
		}
		// draw this element.
		r.draw(*m_foreImg, finalRect, 1.f, clip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }

	Progress::render(finalRect, finalClip);
}

void ImageBar::init(xml::node& node)
{
	Progress::init(node);
	
	xml::node frame = node("Bar");
	if(!frame.empty())
	{
		std::string setname = frame["Imageset"].value();
		m_imgset = m_system.getWindowManager().loadImageset(setname);
		if(m_imgset)
		{
			const Imageset& set = *m_imgset;
			m_backImg = set[frame("Background")["Image"].value()];
			m_foreImg = set[frame("Foreground")["Image"].value()];
		}
	}
}

}