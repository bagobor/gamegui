#include "stdafx.h"
#include "texmanager.h"
#include "texture.h"
#include "renderer.h"

namespace gui
{

TextureManager::TextureManager(Renderer& render) :
	m_render(render)
{
}

TextureManager::~TextureManager()
{
}

TexturePtr TextureManager::createTexture(const std::string& filename)
{	
	TexturesIter i = m_textures.find(filename);
	if(i == m_textures.end())
	{
		TexturePtr tex = m_render.createTextureInstance(filename);
		TextureWeakPtr weak_tex = tex;
		m_textures.insert(std::make_pair(filename, weak_tex));
		return tex;
	}

	TextureWeakPtr weak_tex = i->second;
	if (weak_tex.expired())
	{
		TexturePtr tex = m_render.createTextureInstance(filename);
		weak_tex = tex;
		return tex;
	}

	return weak_tex.lock();
}

void TextureManager::pushTexture(TexturePtr tex)
{
	//TODO: add mem statistics
}

void TextureManager::cleanup()
{
	m_textures.clear();
}

}