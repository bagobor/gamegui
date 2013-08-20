#include "StdAfx.h"
#include "windowmanager.h"

#include "imagesetmanager.h"
#include "font.h"
#include "Renderer.h"
#include "factory.h"
#include "window.h"
#include "system.h"

#include "panel.h"
#include "label.h"
#include "imagebox.h"
#include "editbox.h"
#include "button.h"
#include "checkbox.h"
#include "framewindow.h"
#include "progress.h"
#include "slider.h"
#include "scrollpane.h"
#include "list.h"
#include "combobox.h"
#include "markuptext.h"
#include "chatwindow.h"

namespace gui
{

WindowManager::WindowManager(System& sys, const std::string& scheme) :
	m_system(sys),
	m_scheme(scheme)
{
	sys.logEvent(log::system, "The WindowManager was created");
	m_factory.reset(new WindowFactory(sys));

	sys.logEvent(log::system, "Registering window factory creators");

	m_factory->RegisterCreator("root", CreatorPtr(new RootCreator(sys, "root")));
	m_factory->RegisterCreator<base_window>();	
	m_factory->RegisterCreator<Label>();
	m_factory->RegisterCreator<ImageBox>();
	m_factory->RegisterCreator<Button>();
	m_factory->RegisterCreator<ImageButton>();
	m_factory->RegisterCreator<Thumb>();
	m_factory->RegisterCreator<ScrollThumb>();
	m_factory->RegisterCreator<Slider>();
	m_factory->RegisterCreator<ScrollBar>();
	m_factory->RegisterCreator<Checkbox>();
	m_factory->RegisterCreator<Editbox>();
	m_factory->RegisterCreator<KeyBinder>();
	m_factory->RegisterCreator<FrameWindow>();
	m_factory->RegisterCreator<Panel>();
	m_factory->RegisterCreator<Progress>();
	m_factory->RegisterCreator<ProgressBar>();
	m_factory->RegisterCreator<ImageBar>();
	m_factory->RegisterCreator<ScrollPane>();
	m_factory->RegisterCreator<BaseList>();
	m_factory->RegisterCreator<ListBox>();
	m_factory->RegisterCreator<CategorizedList>();
	m_factory->RegisterCreator<Combobox>();
	m_factory->RegisterCreator<MarkupBase>();
	m_factory->RegisterCreator<MarkupText>();
	m_factory->RegisterCreator<ChatWindow>();
	m_registredTypes = m_factory->GetTypesList();	
}

WindowManager::~WindowManager(void)
{
}

void WindowManager::reset(bool complete)
{
	if(complete){
		m_defaultImageset.reset();
		m_defaultFont.reset();
		m_imagesetRegistry.clear();
		m_fontRegistry.clear();
	}
	m_docCache.clear();
	m_loadedLuaFiles.clear();

	m_system.logEvent(log::system, "Loading system defaults");
	loadScheme(m_scheme);
}

void WindowManager::loadScheme(const std::string& scheme)
{
	if(scheme.empty())
		return;

	std::string filename = "scheme\\" + scheme + ".scheme";

	XmlDocumentPtr pdoc = loadCachedXml(filename);
	if(!pdoc) return;

	xml::node schemenode = pdoc->child("Scheme");
	if(schemenode.empty()) return;


	std::string file = schemenode("DefaultImageset")["file"].value();
	if(!file.empty())
	{
		if (m_defaultImageset = createImageset(file))
			m_imagesetRegistry[m_defaultImageset->GetName()] = m_defaultImageset;
	}

	file = schemenode("DefaultFont")["file"].value();
	if(!file.empty())
	{
		m_defaultFont = createFont(file);
		m_fontRegistry[m_defaultFont->getName()] = m_defaultFont;
	}
}

ImagesetPtr WindowManager::loadImageset(const std::string& name)
{
	if(name.empty())
		return m_defaultImageset;

	ImagesetIt it = m_imagesetRegistry.find(name);
	if(it != m_imagesetRegistry.end())
		return it->second;

	ImagesetPtr imgset = createImageset(name + ".imageset");
	if(imgset)
	{
		m_system.logEvent(log::system, std::string("The imageset ") + name + " succefuly loaded.");
		m_imagesetRegistry[name] = imgset;
		return imgset;
	}
	m_system.logEvent(log::warning, std::string("The imageset ") + name + " cannot be loaded.");
	return ImagesetPtr();
}

ImagesetPtr WindowManager::createImageset(const std::string& filename)
{
	ImagesetPtr retval;
	//std::string path =  m_system.getRenderer().getResourcePath() + "imageset\\";
	//std::string path = "imageset\\";
	
	std::string file = "imageset\\" + filename;

	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(!pdoc)
		return retval;
	xml::node imgsetnode = pdoc->child("Imageset");
	if(!imgsetnode.empty())
	{
		retval = m_imgseManager.Make(m_system, &imgsetnode);
	}
	
	return retval;
}

Imageset* WindowManager::getImageset(std::string name)
{
	return loadImageset(name).get();
}

FontPtr WindowManager::loadFont(const std::string& name)
{
	if(name.empty())
		return m_defaultFont;

	FontIt it = m_fontRegistry.find(name);
	if(it != m_fontRegistry.end())
		return it->second;

	FontPtr font = createFont(name + ".font");
	if(font)
	{
		m_system.logEvent(log::system, std::string("The font ") + name + " successfully loaded.");
		m_fontRegistry[name] = font;
		return font;
	}
	m_system.logEvent(log::error, std::string("The font ") + name + " cannot be loaded.");
	return FontPtr();
}

FontPtr WindowManager::createFont(const std::string& filename)
{
	FontPtr retval;
	//std::string path = m_system.getRenderer().getResourcePath();
	//path += "font\\";

	std::string file = "font\\" + filename;

	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(!pdoc)
		return retval;
		
	xml::node fontnode = pdoc->child("Font");
	if(!fontnode.empty())
	{
		std::string fontname = fontnode["Name"].value();
		std::string fontfile = fontnode["Filename"].value();
		std::string fonttype = fontnode["Type"].value();
		int fontsize = fontnode["Size"].as_int();
		
		try
		{
			retval = m_system.getRenderer().createFont(fontname, "font\\" + fontfile, fontsize);
		}
		catch(std::exception& e)
		{
			retval.reset();

			m_system.logEvent(log::error, e.what());
		}
	}
	
	return retval;
}

Font* WindowManager::getFont(std::string name)
{
	return loadFont(name).get();
}

window_ptr WindowManager::createWindow(const std::string& type, const std::string& name)
{
	return m_factory->Create(type, name);
}

window_ptr WindowManager::loadXml(const std::string& filename)
{
	WindowVector newWindows;
	window_ptr w = createFromFile(filename, newWindows);
	
	if(!newWindows.empty())
	{
		std::for_each(newWindows.rbegin(), newWindows.rend(), boost::bind(&base_window::onLoad, _1));
	}
	return w;
}

void WindowManager::loadLeafWindow(window_ptr wnd, const std::string& xml)
{
	//std::string file = m_system.getRenderer().getResourcePath();
	//file += xml;
	std::string file = xml;
	
	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(pdoc)
	{
		loadLuaFile(xml);
		xml::node n = pdoc->first_child();
		loadWindowProperties(wnd, n);
		loadWindowEvents(wnd, n);
	}
}

window_ptr WindowManager::createFromFile(const std::string& filename, WindowVector& newWindows)
{
	window_ptr root;
	//std::string file = m_system.getRenderer().getResourcePath();
	//file += filename;
	std::string file = filename;
	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(pdoc)
	{
		loadLuaFile(filename);
		xml::node window = pdoc->first_child(); //find root
		if(!window.empty() && isWindowNode(window))
		{
			root = createWindow(window_ptr(), window, newWindows);
		}
	}
	else
	{
		m_system.logEvent(log::warning, std::string("The file ") + filename + " wasn't found or corrupted");
	}

	return root;
}

window_ptr WindowManager::createWindow(window_ptr parent, xml::node& n, WindowVector& newWindows)
{
	window_ptr wnd;
	std::string type(n.name());
	std::string name(n["name"].value());
	std::string link(n["file"].value());

	if(parent)
	{
		wnd = parent->child(name);
		if(!wnd)
		{
			if(!link.empty())
			{
				wnd = createFromFile(link, newWindows);
			}
			else
			{
				wnd = createWindow(type, name);
				if(wnd)
					newWindows.push_back(wnd);
			}
			if(wnd)
			{
				std::string t(wnd->getType());
				if(type != t)
				{
					m_system.logEvent(log::error, std::string("The specified child type is mismatch existing one. Skipping child ") + name);
					return window_ptr();
				}
			}
			parent->add(wnd);
		}
		else
		{
			if(!link.empty())
			{
				m_system.logEvent(log::error, std::string("An overriding the child type information is unacceptable. Skipping child ") + name);
				return window_ptr();
			}
			std::string t(wnd->getType());
			if(type != t)
			{
				m_system.logEvent(log::error, std::string("The specified child type is mismatch existing one. Skipping child ") + name);
				return window_ptr();
			}
		}
	}
	else
	{
		if(type != "root")
		{
			if(!link.empty())
			{
				wnd = createFromFile(link, newWindows);
			}
			else
			{
				wnd = createWindow(type, name);
				if(wnd)
					newWindows.push_back(wnd);
			}
		}
		else
		{
			wnd = createWindow(type, name);
		}
	}
	if(wnd)
	{
		if(type != "root")
		{
			wnd->setName(name);
			loadWindowProperties(wnd, n);
		}
		loadWindowEvents(wnd, n);

		xml::node children = n("Children");
		if(wnd->isCanHaveChildren() && !children.empty())
		{
			xml::node child = children.first_child();
			while(!child.empty())
			{
				if(isWindowNode(child))
				{
					createWindow(wnd, child, newWindows);
				}
				child = child.next_sibling();
			}
		}
	}

	return wnd;
}
void WindowManager::loadWindowProperties(window_ptr wnd, xml::node& n)
{
	xml::node properties = n("Properties");
	if(!properties.empty())
		wnd->init(properties);
}
void WindowManager::loadWindowEvents(window_ptr wnd, xml::node& n)
{
	xml::node events = n("Events");
	if(!events.empty())
		wnd->parseEventHandlers(events);
}

XmlDocumentPtr WindowManager::loadCachedXml(const std::string& file)
{
	XmlDocumentPtr retval;
	
	DocumentCacheIt cache = m_docCache.find(file);
	if(cache != m_docCache.end())
	{
		retval = cache->second;
	}
	else
	{	
		retval.reset(new xml::document());

		std::string data = m_system.get_filesystem()->load_text(file);

		if(!retval->load(data.c_str()))
			retval.reset();
		else
			m_docCache[file] = retval;
	}
	return retval;
}

bool WindowManager::isWindowNode(xml::node& node) const
{
	std::string type(node.name());
	if(!type.empty())
	{
		TypesList::const_iterator it = std::find(m_registredTypes.begin(), m_registredTypes.end(), type);
		if(it != m_registredTypes.end())
			return true;
	}
	return false;
}

void WindowManager::loadLuaFile(const std::string& xmlfile)
{
	if(xmlfile.empty())
		return;
	size_t delim = xmlfile.find_last_of(".");
	std::string name;
	if(delim != std::string::npos)
		name = xmlfile.substr(0, delim);
	else
		name = xmlfile;

	if(!name.empty())
	{		
		name += ".lua";
		LuaFilesVector::iterator it = std::find(m_loadedLuaFiles.begin(), m_loadedLuaFiles.end(), name);
		if(it == m_loadedLuaFiles.end())
		{
			m_loadedLuaFiles.push_back(name);
			//std::string file = m_system.getRenderer().getResourcePath();
			//file += name;
			std::string file = name;
			m_system.executeScript(file);
		}
	}
}

void WindowManager::onLoaded(window_ptr wnd)
{
	if(wnd)
	{
		wnd->onLoad();
		
		base_window::children_list& children = wnd->getChildren();
		base_window::child_iter i = children.begin();
		base_window::child_iter end = children.end();
		while(i != end)
		{
			onLoaded((*i));
			++i;
		}
	}
}

}
