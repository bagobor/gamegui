#pragma once

#include "size.h"

namespace gui
{
	class Renderer;


	class Texture
	{
	protected:
		explicit Texture(Renderer& owner) : m_owner(owner) {}

	public:
		// texture format flags
		enum PixelFormat
		{   
			PF_INVALID = 0x0,
			PF_ALPHA = 0x1,
			PF_RGB = 0x2, //! Each pixel is 3 bytes. RGB in that order.
			PF_RGBA = PF_RGB | PF_ALPHA,//! Each pixel is 4 bytes. RGBA in that order.
			PF_COMPRESSED = 0x4, // platform dependent
			PF_COMPRESSED_ALPHA = PF_COMPRESSED | PF_ALPHA,
			PF_MASK = 0xFF
		};
		
		virtual ~Texture() {}

		const Size& getSize() const { return m_size; }
		virtual float getWidth(void) const { return m_size.width; }
		virtual float getHeight(void) const { return m_size.height; }

		Renderer&	getRenderer(void) const { return m_owner; }
		PixelFormat getPixelFormat() const { return m_format; }

		virtual void update(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) = 0;
		
	protected:
		Texture& operator=(const Texture&) { return *this; }

		void setPixelFormat(PixelFormat fmt) { m_format = fmt; }	

	protected:
		Renderer& m_owner;
		PixelFormat m_format;
		Size m_size;
		std::string m_filename;
		std::string m_resourceGroup;
	};

	typedef std::shared_ptr<Texture> TexturePtr;
}