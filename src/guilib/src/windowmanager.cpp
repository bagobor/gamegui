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


namespace gui
{

WindowManager::WindowManager(System& sys, const std::string& scheme) :
	m_system(sys),
	m_scheme(scheme)
{
	sys.logEvent(log::system, "The WindowManager was created");
	m_factory.reset(new WindowFactory(sys));

	sys.logEvent(log::system, "Registering window factory creators");

	m_factory->registerCreator("root", CreatorPtr(new RootCreator(sys, "root")));
	m_factory->registerCreator<WindowBase>();	
	m_factory->registerCreator<Label>();
	m_factory->registerCreator<ImageBox>();
	m_factory->registerCreator<Button>();
	m_factory->registerCreator<ImageButton>();
	m_factory->registerCreator<Thumb>();
	m_factory->registerCreator<ScrollThumb>();
	m_factory->registerCreator<Slider>();
	m_factory->registerCreator<ScrollBar>();
	m_factory->registerCreator<Checkbox>();
	m_factory->registerCreator<Editbox>();
	m_factory->registerCreator<KeyBinder>();
	m_factory->registerCreator<FrameWindow>();
	m_factory->registerCreator<Panel>();
	m_factory->registerCreator<Progress>();
	m_factory->registerCreator<ProgressBar>();
	m_factory->registerCreator<ImageBar>();
	m_factory->registerCreator<ScrollPane>();
	m_factory->registerCreator<BaseList>();
	m_factory->registerCreator<ListBox>();
	m_factory->registerCreator<CategorizedList>();
	m_factory->registerCreator<Combobox>();
	m_factory->registerCreator<MarkupBase>();
	m_factory->registerCreator<MarkupText>();
	m_registredTypes = m_factory->getTypesList();	
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
		m_imagesetsManager.reset();
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
		if (m_defaultImageset = loadImageset(file))
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
	
	std::string file = "imageset/" + filename;

	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(!pdoc)
		return retval;
	xml::node imgsetnode = pdoc->child("Imageset");
	if(!imgsetnode.empty())
	{
		retval = m_imagesetsManager.create(m_system, imgsetnode);
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

	std::string file = "font/"+ filename;

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

	using namespace std::placeholders;
	
	if(!newWindows.empty())
	{
		std::for_each(newWindows.rbegin(), newWindows.rend(), std::bind(&WindowBase::onLoad, _1));
	}

	if (w)
		w->onLoad();

	return w;
}

void WindowManager::loadLeafWindow(window_ptr wnd, const std::string& xml)
{
	std::string file = xml;
	
	XmlDocumentPtr pdoc = loadCachedXml(file);
	if(pdoc)
	{		
		xml::node n = pdoc->first_child();
		loadWindowProperties(wnd, n);
		loadWindowEvents(wnd, n);
		loadLuaFile(xml, wnd);
	}
}

window_ptr WindowManager::createFromFile(const std::string& filename, WindowVector& newWindows)
{
	//std::string file = m_system.getRenderer().getResourcePath();
	//file += filename;
	XmlDocumentPtr pdoc = loadCachedXml(filename);
	if (!pdoc)
	{
		m_system.logEvent(log::warning, std::string("The file ") + filename + " wasn't found or corrupted");
		return window_ptr();
	}
	xml::node window = pdoc->first_child(); //find root
	
	if(window.empty() || !isWindowNode(window))
		return window_ptr();

	window_ptr root = createWindow(window_ptr(), window, newWindows);
	loadLuaFile(filename, root);
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
		if(wnd->canHaveChildren() && !children.empty())
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
	if (properties.empty()) return;
	wnd->init(properties);
}
void WindowManager::loadWindowEvents(window_ptr wnd, xml::node& n)
{
	xml::node events = n("Events");
	if (events.empty()) return;
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

		std::string data = m_system.filesystem()->load_text(file);

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

void WindowManager::loadLuaFile(const std::string& xmlfile, window_ptr wnd) {
	if (xmlfile.empty()) return;

	size_t delim = xmlfile.find_last_of(".");
	std::string filename = delim != std::string::npos ? xmlfile.substr(0, delim) : xmlfile;

	if (filename.empty()) return;
	
	filename += ".lua";
	LuaFilesVector::iterator it = std::find(m_loadedLuaFiles.begin(), m_loadedLuaFiles.end(), filename);
	//if (it != m_loadedLuaFiles.end()) return;

	//m_loadedLuaFiles.push_back(filename);
	//std::string file = m_system.getRenderer().getResourcePath();
	//file += name;
	m_system.executeScript(filename, wnd.get());
}

void WindowManager::loadLuaFile(const std::string& xmlfile)
{
	if (xmlfile.empty()) return;

	size_t delim = xmlfile.find_last_of(".");
	std::string filename = delim != std::string::npos ? xmlfile.substr(0, delim) : xmlfile;

	if (filename.empty()) return;

	filename += ".lua";
	LuaFilesVector::iterator it = std::find(m_loadedLuaFiles.begin(), m_loadedLuaFiles.end(), filename);
	if (it != m_loadedLuaFiles.end()) return;

	m_loadedLuaFiles.push_back(filename);
	//std::string file = m_system.getRenderer().getResourcePath();
	//file += name;
	m_system.executeScript(filename);
}

void WindowManager::onLoaded(window_ptr wnd)
{
	if(wnd)
	{
		wnd->onLoad();
		
		WindowBase::children_t& children = wnd->getChildren();
		WindowBase::child_iter i = children.begin();
		WindowBase::child_iter end = children.end();
		while(i != end)
		{
			onLoaded((*i));
			++i;
		}
	}
}

}
