#include "stdafx.h"

#include "markuptext.h"

#include "system.h"
#include "renderer.h"
#include "imagesetmanager.h"
#include "tooltip.h"
#include "utils.h"
#include "eventtypes.h"
#include "font.h"

namespace gui
{

	void MarkupBase::TextLine::addChunk(PText chunk, float spacing)
	{
		if(chunk && chunk->len)
		{
			children.push_back(chunk);
			chunk->parent = this;

			float newheight = spacing * chunk->font->getLineSpacing();
			if(area.getHeight() < newheight)
				area.setHeight(newheight);
		}
	}

	size_t getNextTokenLength(const std::string& text, size_t start_idx, bool& isWhiteSpace)
	{
		size_t pos = text.find_first_of(" \n\t\r", start_idx);

		// handle case where no more whitespace exists (so this is last token)
		if (pos == std::string::npos)
		{
			return (text.length() - start_idx);
		}
		// handle 'delimiter' token cases
		else if ((pos - start_idx) == 0)
		{
			size_t wstart = start_idx + 1;
			size_t wstop = start_idx + 1;
			do
			{
				wstart = wstop;
				wstop = text.find_first_of(" \n\t\r", wstart);
				if(wstop == wstart)
					break;
			}
			while(wstop - wstart <= 1);
			
			isWhiteSpace = true;

			return wstart - start_idx;
		}
		else
		{
			return (pos - start_idx);
		}

	}

	MarkupBase::MarkupBase(System& sys, const std::string& name) :
		Label(sys, name),
		m_textSpacing(1.2f),
		m_listSpacing(2.0f),
		m_minWidth(40.f),
		m_margin(4.f, 4.f, 4.f, 4.f),
		m_parser(sys)
	{
	}

	MarkupBase::~MarkupBase(void)
	{
		clear();
	}

	void MarkupBase::setText(const std::string& text)
	{
		Label::setText(text);
		if(text.empty())
			return;
		
		m_parser.parseText(text, m_foreColor, m_font);

		m_viewport = m_area;
		m_viewport.m_left += m_margin.m_left;
		m_viewport.m_right -= m_margin.m_right;
		m_viewport.m_top += m_margin.m_top;
		m_viewport.m_bottom -= m_margin.m_bottom;
		if(m_viewport.m_right - m_viewport.m_left < m_minWidth)
			return;

		parseBBCodes();
	}

	void MarkupBase::parseBBCodes()
	{
		clear();
		const float maxwidth = m_viewport.getWidth();
		float height = 0.f;
		
		float xpos = 0.f;
		size_t wordcount = 0;
		PTextLine currentLine;
		PText textChunk;
		PImg imgChunk;
		PTooltipArea tooltipChunk;
		PLinkArea urlChunk;
		float linespacing = m_textSpacing;

		const ActiveText::TextViewVector& v = m_parser.getFormattedOutput();
		ActiveText::TextViewVector::const_iterator i = v.begin();
		ActiveText::TextViewVector::const_iterator end = v.end();
		while(i != end)
		{
			const ActiveText::TextView* tv = (*i).get();
			size_t pos = tv->start;
			size_t len = tv->len;
			size_t last = pos + len;
			std::string text = m_text.substr(pos, len);
			
			if(tv->isNewLine)
			{
				height += addTextLine(currentLine);

				// new paragraph
				currentLine.reset(new TextLine());
				currentLine->area = m_viewport;
				currentLine->area.m_top = height;
				currentLine->area.setHeight(0.f);
				linespacing = tv->isList ? m_listSpacing : m_textSpacing;
				wordcount = 0;
				xpos = 0.f;
				
				xpos += m_font->getTextExtent("    ");
			}

			{
				textChunk.reset(new Text());
				textChunk->start = pos;
				textChunk->col = tv->col;
				textChunk->font = tv->font;
				textChunk->selcol = tv->col;
				textChunk->selcol.invertColour();
				textChunk->area.m_left = xpos;
				textChunk->area.setHeight(textChunk->font->getLineSpacing());
				textChunk->area.setWidth(0.f);
			}

			if(tv->isTooltip)
			{
				if(tooltipChunk)
				{
					if(tooltipChunk->parent != tv->tooltipnode)
					{
						m_tooltips.push_back(tooltipChunk);
						tooltipChunk.reset(new TooltipArea());
						tooltipChunk->parent = tv->tooltipnode;
						tooltipChunk->tooltip = tv->tooltip;
					}
				}
				else
				{
					tooltipChunk.reset(new TooltipArea());
					tooltipChunk->parent = tv->tooltipnode;
					tooltipChunk->tooltip = tv->tooltip;
				}
			}

			if(tv->isURL)
			{
				if(urlChunk)
				{
					if(urlChunk->parent != tv->urlnode)
					{
						m_links.push_back(urlChunk);
						urlChunk.reset(new LinkArea());
						urlChunk->parent = tv->urlnode;
						urlChunk->type = tv->type;
						urlChunk->id = tv->id;
					}
				}
				else
				{
					urlChunk.reset(new LinkArea());
					urlChunk->parent = tv->urlnode;
					urlChunk->type = tv->type;
					urlChunk->id = tv->id;
				}
			}

			if(!tv->isImage)
			{ // text
				while(pos < last)
				{
					bool white = false;
					size_t nextTokenLen = getNextTokenLength(m_text, pos, white);
					if(pos + nextTokenLen > last)
						nextTokenLen = last - pos;

					float xlen = tv->font->getTextExtent(m_text.substr(pos, nextTokenLen));

					// if text isn't fit anyway (and must be alone on a string)
					if(xlen > maxwidth && wordcount == 0)
						xlen = maxwidth - xpos;

					if(xpos + xlen > maxwidth)
					{
						textChunk->len = pos - textChunk->start;
						if(white)
						{
							// don't allow whitespace in new line
							pos += nextTokenLen;
						}

						currentLine->addChunk(textChunk, linespacing);
						if(tv->isTooltip)
							tooltipChunk->masked.push_back(textChunk);
						if(tv->isURL)
							urlChunk->masked.push_back(textChunk);

						height += addTextLine(currentLine);
						
						{
							// new line
							currentLine.reset(new TextLine());
							currentLine->area = m_viewport;
							currentLine->area.m_top = height;
							currentLine->area.setHeight(0.f);
							linespacing = tv->isList ? m_listSpacing : m_textSpacing;
							wordcount = 0;
							xpos = 0.f;
						}

						{
							textChunk.reset(new Text());
							textChunk->start = pos;
							textChunk->col = tv->col;
							textChunk->font = tv->font;
							textChunk->selcol = tv->col;
							textChunk->selcol.invertColour();
							textChunk->area.m_left = xpos;
							textChunk->area.setHeight(textChunk->font->getLineSpacing());
							textChunk->area.setWidth(0.f);
						}
					}
					else
					{
						// add next word until width or tag
						pos += nextTokenLen;
						xpos += xlen;
						wordcount++;
						textChunk->area.m_right = xpos;
					}
				}

				// end chunk
				textChunk->len = pos - textChunk->start;
				currentLine->addChunk(textChunk, linespacing);
				if(tv->isTooltip)
					tooltipChunk->masked.push_back(textChunk);
				if(tv->isURL)
					urlChunk->masked.push_back(textChunk);
			}
			else
			{ // image
				imgChunk.reset(new Img());
				const Image* img = tv->img;			
				imgChunk->img = img;
				
				if(img)
				{
					Size imgsize = img->GetSize();

					if(imgsize.width > maxwidth - xpos)
					{
						height += addTextLine(currentLine);
						
						{
							// new line
							currentLine.reset(new TextLine());
							currentLine->area = m_viewport;
							currentLine->area.m_top = height;
							currentLine->area.setHeight(0.f);
							linespacing = tv->isList ? m_listSpacing : m_textSpacing;
							wordcount = 0;
							xpos = 0.f;
						}					
					}

					imgChunk->area.m_left = m_viewport.m_left + xpos;
					imgChunk->area.m_top = /*m_viewport.m_top*/ + currentLine->area.m_top;
					imgChunk->area.m_bottom = imgChunk->area.m_top + imgsize.height;
					imgChunk->area.m_right = imgChunk->area.m_left + imgsize.width;
					xpos += imgsize.width;
					wordcount++;

					currentLine->addChunk(imgChunk);
					m_images.push_back(imgChunk);

					if(tv->isTooltip)
						tooltipChunk->maskedimg.push_back(imgChunk);
					if(tv->isURL)
						urlChunk->maskedimg.push_back(imgChunk);
				}			
			}

			++i;
		}
		
		if(currentLine && !currentLine->isEmpty())
			height += addTextLine(currentLine);
		if(tooltipChunk && !tooltipChunk->isEmpty())
			m_tooltips.push_back(tooltipChunk);
		if(urlChunk && !urlChunk->isEmpty())
			m_links.push_back(urlChunk);

		m_viewport.m_bottom = m_viewport.m_top + height;
		m_area.m_bottom = m_viewport.m_bottom + m_margin.m_bottom;
	}

	float MarkupBase::addTextLine(PTextLine line)
	{
		if(line && !line->isEmpty())
		{
			line->alignContent();
			m_textlines.push_back(line);
			return line->area.getHeight();
		}
		return 0.f;
	}

	void MarkupBase::clear()
	{
		m_textlines.clear();
		m_images.clear();
		m_tooltips.clear();
		m_links.clear();
	}


	bool MarkupBase::onLoad(void)
	{
		return Label::onLoad();
	}

	bool MarkupBase::onSized(bool update)
	{
		if(update)
		{
			Rect area(m_area);
			area.m_left += m_margin.m_left;
			area.m_right -= m_margin.m_right;
			area.m_top += m_margin.m_top;
			area.m_bottom -= m_margin.m_bottom;
			if(area.m_right - area.m_left >= m_minWidth)
			{
				if(m_viewport != area)
				{
					clear();
					m_viewport = area;
					parseBBCodes();
				}
			}
			else
				clear();
		}
		return base_window::onSized(false);
	}


	void MarkupBase::render(const Rect& finalRect, const Rect& finalClip)
	{
		size_t len = m_text.length();
		std::vector<PTextLine>::const_iterator i = m_textlines.begin();
		std::vector<PTextLine>::const_iterator end = m_textlines.end();
		while(i != end)
		{
			const TextLine* line = (*i).get();
			size_t offset = line->start;
			Rect rl(line->area);

			std::vector<PText>::const_iterator c = line->children.begin();
			std::vector<PText>::const_iterator stop = line->children.end();
			while(c != stop)
			{
				const Text* chunk = (*c).get();
				if(chunk->len > 0 && chunk->len <= len)
				{
					FontPtr f = chunk->font;
					Color col = chunk->selected ? chunk->selcol : chunk->col;
					std::string buf = m_text.substr(chunk->start + offset, chunk->len);
					Rect rc(chunk->area);
					rc.offset(rl.getPosition());
					rc.offset(finalRect.getPosition());
					f->drawText(buf, rc, 1.0f, finalClip, LeftAligned, col, 1.f, 1.f);
				}

				++c;
			}
			++i;
		}
		Renderer& r = m_system.getRenderer();

		std::vector<PImg>::const_iterator cimg = m_images.begin();
		std::vector<PImg>::const_iterator cimgend = m_images.end();
		while(cimg != cimgend)
		{
			const Img* img = (*cimg).get();
			Rect rc(img->area);
			rc.offset(finalRect.getPosition());
			if (img->img)
			{
				r.draw(*img->img, rc, 1.f, finalClip,  m_backColor, Stretch, Stretch);
			}
			++cimg;
		}
	}

	void MarkupBase::init(xml::node& node)
	{
		Label::init(node);

		xml::node setting = node("TextSpacing");
		if(!setting.empty())
		{
			m_textSpacing = (float)atof(setting.first_child().value());
		}

		setting = node("ListSpacing");
		if(!setting.empty())
		{
			m_listSpacing = (float)atof(setting.first_child().value());
		}
		
		setting = node("MinWidth");
		if(!setting.empty())
		{
			m_minWidth = (float)atof(setting.first_child().value());
		}
		setting = node("Margin");
		if(!setting.empty())
		{
			m_margin = StringToRect(setting.first_child().value());
		}
		setting = node("Text");
		if(!setting.empty())
		{
			setText(setting.first_child().value());
		}
	}

	MarkupText::MarkupText(System& sys, const std::string& name) :
		MarkupBase(sys, name)
	{
	}

	MarkupText::~MarkupText(void)
	{
		clear();
	}

	void MarkupText::showTooltip(PTooltipArea p)
	{
		if(p)
		{
			m_selectedtt = p;
			MarkupEventArgs m;
			m.name = "On_MarkupTooltipShow";
			m.id = (unsigned int)p->tooltip;
			callHandler(&m);

			m_system.getTooltip()->show();
		}
	}

	void MarkupText::hideTooltip()
	{
		if(m_selectedtt)
		{
			MarkupEventArgs m;
			m.name = "On_MarkupTooltipHide";
			m.id = (unsigned int)m_selectedtt->tooltip;
			callHandler(&m);

			m_system.getTooltip()->reset();
		}
		m_selectedtt.reset();
	}

	bool MarkupText::onMouseEnter(void)
	{
		hideTooltip();
		m_selectedlnk.reset();
		invalidate();
		return true;
	}

	bool MarkupText::onMouseLeave(void)
	{
		hideTooltip();
		m_selectedlnk.reset();
		invalidate();
		return true;
	}

	bool MarkupText::onMouseMove(void)
	{
		m_pt = transformToWndCoord(m_system.getCursor().getPosition());
		m_pt -= m_area.getPosition();

		bool linkhit = false;
		if(m_selectedlnk)
		{	
			linkhit = isHitChunk<LinkArea>(m_selectedlnk.get(), m_pt);
			if(!linkhit)
			{
				setHovered(m_selectedlnk.get(), false);
				m_selectedlnk.reset();
				invalidate();
			}
		}

		if(!m_selectedlnk)
		{
			std::vector<PLinkArea>::iterator ilink = std::find_if(m_links.begin(), m_links.end(), hovertester<LinkArea>(m_pt, *this));
			if(ilink != m_links.end())
			{
				m_selectedlnk = (*ilink);
				setHovered(m_selectedlnk.get(), true);
				invalidate();
			}
		}

		if(m_selectedtt)
		{	
			bool hit = isHitChunk<TooltipArea>(m_selectedtt.get(), m_pt);
			if(hit)
				return true;
			else
			{
				hideTooltip();
			}
		}

		std::vector<PTooltipArea>::iterator itt = std::find_if(m_tooltips.begin(), m_tooltips.end(), hovertester<TooltipArea>(m_pt, *this));
		if(itt != m_tooltips.end())
		{
			showTooltip(*itt);
		}

		return MarkupBase::onMouseMove();
	}

	bool MarkupText::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(m_selectedlnk)
		{
			MarkupEventArgs m;
			m.name = "On_MarkupMouseButton";
			m.mbutton = btn;
			m.mstate = state;
			m.id = (unsigned int)m_selectedlnk->id;
			m.type = m_selectedlnk->type.c_str();
			callHandler(&m);
			return m.handled;
		}
		return MarkupBase::onMouseButton(btn, state);
	}

	template<typename T>
	bool MarkupText::isHitChunk(T* p, point& pt)
	{
		if(!p) return false;
		std::vector<PImg>::iterator img = std::find_if(p->maskedimg.begin(), p->maskedimg.end(), hittester<Img>(pt));
		if(img != p->maskedimg.end())
			return true;

		std::vector<PText>::iterator text = std::find_if(p->masked.begin(), p->masked.end(), hittester<Text>(pt));
		if(text != p->masked.end())
			return true;
			
		return false;
	}

	void MarkupText::setHovered(LinkArea* p, bool status)
	{
		std::for_each(p->masked.begin(), p->masked.end(), hoversetter(status));
	}

	void MarkupText::setTooltipHover(TooltipArea* p)
	{
	}

	void MarkupText::clear()
	{
		m_selectedtt.reset();
		m_selectedlnk.reset();
		MarkupBase::clear();
	}
}