#pragma once

#include "label.h"
#include "ActiveText.h"

namespace gui
{
	class Image;

	class MarkupBase : public Label
	{
	public:
		struct TextLine;
		struct Text
		{
			Rect			area;
			size_t			start;
			size_t			len;
			FontPtr			font;
			Color			col;
			bool			selected;
			Color			selcol;
			TextLine*		parent;
			Text() : area(0.f, 0.f, 0.f, 0.f), start(0), len(0), col(1.f, 1.f, 1.f), selected(false), selcol(1.f, 1.f, 1.f), parent(0) {}
			bool isHit(point& pt) 
			{ 
				if(!parent)
					return false;
				Rect rc(area);
				rc.offset(parent->area.getPosition());
				return rc.isPointInRect(pt);
			}
		};

		typedef std::shared_ptr<Text> PText;

		struct Img
		{
			Rect			area;
			const Image*	img;
			Img() : area(0.f, 0.f, 0.f, 0.f) {}
			bool isHit(point& pt) { return area.isPointInRect(pt); }
		};
		typedef std::shared_ptr<Img> PImg;
		

		struct TextLine
		{
			Rect			area;
			size_t			start;
			size_t			len;
			bool			haveImg; // TODO: remove
			std::vector<PText> children;
			std::vector<PImg> images;
			TextLine() : area(0.f, 0.f, 0.f, 0.f), start(0), len(0), haveImg(false) {}
			
			bool isEmpty() { return children.empty() && images.empty(); }
			
			void addChunk(PText chunk, float spacing);
			void addChunk(PImg img)
			{
				if(img)
				{
					images.push_back(img);
					float newheight = img->area.getHeight();
					if(area.getHeight() < newheight)
						area.setHeight(newheight);
				}
			}

			void alignContent()
			{
				float lineheight = area.getHeight();

				std::for_each(children.begin(), children.end(), aligner<Text>(lineheight));
				std::for_each(images.begin(), images.end(), aligner<Img>(lineheight));
			}

			template<typename T>
			struct aligner
			{
				aligner(float f) : height(f) {}
				float height;
				void operator ()(std::shared_ptr<T>& p)
				{
					float h = p->area.getHeight();
					p->area.offset(point(0.f, (height - h)/2));
				}
			};
		};
		typedef std::shared_ptr<TextLine> PTextLine;

		struct TooltipArea
		{
			ActiveText::TextNode*			parent;
			size_t				tooltip;
			std::vector<PText>	masked;
			std::vector<PImg>	maskedimg;
			TooltipArea() : tooltip(0), parent(0) {}

			bool isEmpty() { return masked.empty() && maskedimg.empty(); }
		};
		typedef std::shared_ptr<TooltipArea> PTooltipArea;

		struct LinkArea
		{
			ActiveText::TextNode*		parent;
			std::string		type;
			size_t			id;
			std::vector<PText>	masked;
			std::vector<PImg>	maskedimg;
			LinkArea() : id(0), parent(0) {}

			bool isEmpty() { return masked.empty() && maskedimg.empty(); }
		};
		typedef std::shared_ptr<LinkArea> PLinkArea;
		

		typedef MarkupBase Self;
		MarkupBase(System& sys, const std::string& name = std::string());
		virtual ~MarkupBase();

		static const char* GetType() { return "MarkupBase"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void setText(const std::string& text);

		virtual bool onLoad();
		virtual void init(xml::node& node);
		virtual bool onSized(bool update = true);
		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void clear();

	protected:
		void parseBBCodes();
		float addTextLine(PTextLine line);

	protected:
		float m_textSpacing;
		float m_listSpacing;
		Rect m_margin;
		float m_minWidth;
		Rect m_viewport;

		std::vector<PTextLine>	m_textlines;
		std::vector<PImg>		m_images;
		std::vector<PTooltipArea>	m_tooltips;
		std::vector<PLinkArea>	m_links;

		ActiveText::Parser m_parser;

	};

	class  MarkupText : public MarkupBase
	{
	public:
		typedef MarkupText Self;
		MarkupText(System& sys, const std::string& name = std::string());
		virtual ~MarkupText(void);

		static const char* GetType() { return "MarkupText"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual bool onMouseMove();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseEnter();
		virtual bool onMouseLeave();

		virtual void clear();

	protected:
		MarkupText& operator=(const MarkupText& rhs) {rhs;}
		template<typename T>
		bool isHitChunk(T* p, point& pt);
		void setHovered(LinkArea* p, bool status);
		void setTooltipHover(TooltipArea* p);

		void showTooltip(PTooltipArea p);
		void hideTooltip();

	protected:
		PTooltipArea m_selectedtt;
		PLinkArea m_selectedlnk;
		point m_pt;

		template<typename T>
		struct hittester
		{
			hittester(point& p) : pt(p) {}
			point pt;
			bool operator ()(std::shared_ptr<T>& p)
			{
				return p->isHit(pt);
			}
		};

		template<typename T>
		struct hovertester
		{
			hovertester(point& p, MarkupText& m) : pt(p), parent(m) {}
			point pt;
			MarkupText& parent;
			bool operator ()(std::shared_ptr<T>& p)
			{
				return parent.isHitChunk<T>(p.get(), pt);
			}
		};

		struct hoversetter
		{
			hoversetter(bool s) : status(s) {}
			bool status;
			void operator ()(std::shared_ptr<Text>& p)
			{
				p->selected = status;
			}
		};
	};
}