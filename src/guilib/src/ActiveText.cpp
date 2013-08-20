#include "stdafx.h"
#include "ActiveText.h"

#include "system.h"
#include "windowmanager.h"
#include "utils.h"

namespace gui
{
	namespace ActiveText
	{
		Parser::Parser(System& sys)
			: m_sys(sys)
			, m_paragraph(false)
			, m_list(false)
		{
			m_tagtypes["color"] = ColorTag;
			m_tagtypes["b"] = BoldTag;
			m_tagtypes["i"] = ItalicTag;
			m_tagtypes["*"] = ListTag;
			m_tagtypes["img"] = ImgTag;
			m_tagtypes["url"] = UrlTag;
			m_tagtypes["tt"] = TooltipTag;
		}

		Parser::~Parser()
		{
		}


		void Parser::parseText(const std::string& text, Color c, FontPtr font)
		{
			//clear old markup
			m_formatting.clear();
			m_view.clear();
			m_root.reset(new TextNode());
			m_root->type = NormalTag;

			size_t len = text.length();
			if(!len) return;

			m_color = c;
			m_font = font;
			m_list = false;

			m_root->len = len;
			m_root->content = 0;

			size_t pos = 0;
			size_t linelen = 0;
			while(pos < len)
			{
				size_t newline = text.find_first_of("\n", pos);
				if(newline == std::string::npos)
				{
					newline = len;
					linelen = len - pos;
				}
				else
					linelen = newline - pos;

				PTextNode linenode(new TextNode());
				linenode->start = pos;
				linenode->len = linelen;
				linenode->content = pos;
				linenode->parent = m_root.get();
				linenode->type = NormalTag;		
				m_root->children.push_back(linenode);

				m_paragraph = true;
				size_t last = pos + linelen;
				pos = parseString(text, pos, linelen, linenode);
				if(pos == last)
					m_list = false;

			}
		}

		size_t Parser::parseString(const std::string& text, size_t start, size_t len, PTextNode node)
		{
			size_t pos = node->content;
			size_t last = start + len;

			std::string debug1 = text.substr(pos, len);

			while(pos < last)
			{
				size_t view = pos;
				PTextView chunk(new TextView());
				chunk->col = m_color;
				chunk->font = m_font;

				applyFormatting(chunk, node.get());
				chunk->start = view;		

				pos = text.find_first_of("[", pos);
				if(pos != std::string::npos && pos < last)
				{			
					size_t close = text.find_first_of("]", pos);
					if(close == std::string::npos || close > last)
					{
						pos = last;
						chunk->len = pos - view;
						if(chunk->len || node->type == ImgTag)
						{
							chunk->isNewLine = m_paragraph;
							chunk->isList = m_list;
							m_paragraph = false;
							m_formatting.push_back(chunk);
						}
						m_list = false;
						break; // newline symbol inside a tag!
					}

					chunk->len = pos - view;
					if(chunk->len || node->type == ImgTag)
					{
						chunk->isNewLine = m_paragraph;
						chunk->isList = m_list;
						m_paragraph = false;
						m_formatting.push_back(chunk);
					}

					std::string param;
					std::string tag = text.substr(pos + 1, close - (pos + 1));

					bool closed = false;
					TagType type = parseNodeTag(tag, closed, param);

					if(!closed)
					{
						if(type == ListTag)
						{
							m_list = true;
							if(!m_paragraph)
							{
								m_paragraph = true;
								pos = close;
								break;
							}
						}
						// New tag found
						PTextNode childnode(new TextNode());
						childnode->start = pos;
						childnode->content = close + 1;
						childnode->parent = node.get();
						childnode->type = type;
						childnode->param = param;
						node->children.push_back(childnode);

						pos = parseString(text, pos, last - pos, childnode);
					}
					else
					{
						pos = close;
						if(type == node->type)
						{
							break;
						}
					}
				}
				else
				{
					pos = last;
					chunk->len = pos - view;
					if(chunk->len || node->type == ImgTag)
					{
						chunk->isNewLine = m_paragraph;
						chunk->isList = m_list;
						m_paragraph = false;
						m_formatting.push_back(chunk);
					}
					break;
				}
			}	

			pos++;
			node->len = pos - node->start;
			return pos;
		}

		TagType Parser::parseNodeTag(const std::string tag, bool& closed, std::string& param)
		{
			TagType type = NormalTag;
			size_t start = 0;
			if(tag.at(start) == '/')
			{
				closed = true;
				start = 1;
			}

			size_t n = tag.find_first_of("=", start);
			if(n == std::string::npos)
				n = tag.length();
			else
				param = tag.substr(n+1);

			std::string name = tag.substr(start, n - start);
			type = m_tagtypes[name];

			return type;
		}

		void splitParams_(const std::string& text, std::string& p1, std::string& p2)
		{
			size_t n = text.find_first_of(",");
			if(n == std::string::npos)
				n = text.length();
			else
				p2 = text.substr(n+1);

			p1 = text.substr(0, n);
		}


		void Parser::applyFormatting(PTextView chunk, TextNode* node)
		{
			if(!node) return;
			if(node->parent)
			{
				applyFormatting(chunk, node->parent);
			}

			if(node->param.empty())
				return;

			switch(node->type)
			{
			case ColorTag:
				chunk->col = HexStringToColor(node->param);
				break;
			case BoldTag:
				chunk->isBold = true;
				break;
			case ItalicTag:
				chunk->isItalic = true;
				break;
			case ImgTag:
				{
					chunk->isImage = true;
					std::string set;
					std::string img;
					splitParams_(node->param, set, img);
					ImagesetPtr is = m_sys.getWindowManager().loadImageset(set);
					if(is) chunk->img = (*is)[img];
				}
				break;
			case UrlTag:
				{
					chunk->isURL = true;
					chunk->urlnode = node;
					std::string id;
					splitParams_(node->param, chunk->type, id);
					chunk->id = (unsigned int)atoi(id.c_str());
				}

				break;
			case TooltipTag:
				chunk->isTooltip = true;
				chunk->tooltipnode = node;
				chunk->tooltip = (unsigned int)atoi(node->param.c_str());
				break;

			case NormalTag:
			case ListTag:
			default:
				break;
			};

		}
	}

}