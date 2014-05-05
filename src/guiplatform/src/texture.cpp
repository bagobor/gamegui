#include "stdafx.h"
#include "texture.h"
#include "renderer.h"

using namespace gui;
using namespace ogl_platform;

TextureGL::TextureGL(renderer& owner, size_t tex) :
	Texture(owner)
	,m_texture(tex)
{
	calculateMetrics();
}

TextureGL::~TextureGL()
{
	free_platform_resource();
}

void TextureGL::calculateMetrics()
{
	m_size.width = 0;
	m_size.height = 0;
	m_format = Texture::PF_RGBA;

	//assert(m_texture);

	//m_size.width = m_texture->get_width()*1.0f;
	//m_size.height = m_texture->get_height()*1.0f;

	//rgde::render::surface_ptr surface = m_texture->get_surface(0);

	//if (!surface)
	//	throw std::exception("Failed to load texture from memory: Invalid texture.");

	//using rgde::render::resource;

	//switch (surface->get_format())
	//{
	//case resource::r8g8b8:
	//	m_format = Texture::PF_RGB;
	//    break;

	//case resource::x8r8g8b8:
	//case resource::a8r8g8b8:
	//	m_format = Texture::PF_RGBA;
	//    break;

	//case resource::dxt1:
	//case resource::dxt2:
	//case resource::dxt3:
	//case resource::dxt4:
	//case resource::dxt5:
	//	m_format = Texture::PF_COMPRESSED;
	//	break;

	//default:
	//	throw std::exception("Failed to load texture from memory: Invalid pixelformat.");
	//	break;
	//}
}

void TextureGL::free_platform_resource()
{
	//glTexture
	//m_texture.reset();
}