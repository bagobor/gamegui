#pragma once

#include "color.h"

namespace gui
{
	class System;
	class Image;
	class Font;
	typedef boost::shared_ptr<Font> FontPtr;

	namespace ActiveText
	{
		struct TextNode;
		typedef boost::shared_ptr<TextNode> PTextNode;
		typedef std::vector<PTextNode> TextNodeVector;

		enum TagType
		{
			NormalTag,
			ColorTag,
			BoldTag,
			ItalicTag,
			ListTag,
			ImgTag,
			UrlTag,
			TooltipTag
		};

		struct TextNode
		{	
			size_t start;
			size_t len;
			size_t content;
			TagType type;
			std::string param;
			TextNode* parent;
			TextNodeVector children;
			TextNode() : start(0), len(0), content(0), parent(0), type(NormalTag) {}
		};

		struct TextView
		{
			size_t start;
			size_t len;
			bool isBold;
			bool isItalic;
			FontPtr font;
			Color col;
			bool isNewLine;
			bool isList;
			
			bool isImage;
			const Image* img;

			bool isTooltip;
			TextNode* tooltipnode;
			size_t tooltip;

			bool isURL;
			TextNode* urlnode;
			std::string type;
			size_t id;

			TextView() : 
				start(0),
				len(0),
				isBold(false),
				isItalic(false),
				isImage(false),
				img(0),
				isTooltip(false),
				tooltip(0),
				isURL(false),
				id(0),
				isNewLine(false),
				isList(false),
				tooltipnode(0),
				urlnode(0)
			{}
		};
		typedef boost::shared_ptr<TextView> PTextView;
		typedef std::vector<PTextView> TextViewVector;

		class Parser
		{
		public:
			explicit Parser(System& sys);
			~Parser();

			void parseText(const std::string& text, Color c, FontPtr font);
			
			TextNodeVector& getView() { return m_view; }
			PTextNode getRoot() const { return m_root; }
			TextViewVector& getFormattedOutput() { return m_formatting; }

		protected:
			Parser& operator=(const Parser& rhs) {rhs;}
			size_t parseString(const std::string& text, size_t start, size_t len, PTextNode node);
			TagType parseNodeTag(const std::string tag, bool& closed, std::string& param);
			void applyFormatting(PTextView chunk, TextNode* node);

		protected:
			TextNodeVector m_view;
			PTextNode m_root;
			TextViewVector m_formatting;
			std::map<std::string, TagType> m_tagtypes;

			System& m_sys;
			Color	m_color;
			FontPtr m_font;
			bool	m_paragraph;
			bool	m_list;
		};
	}
}