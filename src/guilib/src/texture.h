#pragma once

#include "size.h"

namespace gui
{
	struct RenderDevice;


	class Texture
	{
	protected:
		explicit Texture(RenderDevice& owner) : m_owner(owner),
			m_format(PF_INVALID),
			m_image_format(DF_RAW),
			m_size(-1,-1)
		{
		}

	public:
		// texture format flags
		enum PixelFormat
		{   
			PF_INVALID = 0x0,
			PF_FLAG_ALPHA = 0x1,
			PF_FLAG_COMPRESSED = 0x02,
			PF_RGB888 = 0x10, //! Each pixel is 3 bytes. RGB in that order.
			PF_RGBA8888 = PF_RGB888 | PF_FLAG_ALPHA,//! Each pixel is 4 bytes. RGBA in that order.
			PF_RGB565 = 0x20, // platform dependent
			PF_RGBA4444 = 0x30 | PF_FLAG_ALPHA, // platform dependent
			PF_RGB5A1 = 0x40 | PF_FLAG_ALPHA,
			PF_PVRTC4 = 0x50 | PF_FLAG_ALPHA | PF_FLAG_COMPRESSED,
			PF_PVRTC2 = 0x60 | PF_FLAG_ALPHA | PF_FLAG_COMPRESSED,

			//! Default texture format: RGBA8888
			PF_Default = PF_RGBA8888,
			
			PF_MASK = 0xFF
		};

		enum ImageFormat
		{
			DF_FLAG_HARDWARE = 0x10,
			DF_JPG = 0,
			DF_PVR_APPLE = 0x1 | DF_FLAG_HARDWARE,
			DF_PVR_IMAGINATION = 0x2 | DF_FLAG_HARDWARE,
			DF_PNG = 0x3,
			DF_WEBP = 0x4,
			DF_RAW = 0x5,			
			DF_UNKNOWN
		};
		
		virtual ~Texture() = default;

		const Size& getSize() const { return m_size; }
		virtual float getWidth(void) const { return m_size.width; }
		virtual float getHeight(void) const { return m_size.height; }

		RenderDevice&	getRenderer(void) const { return m_owner; }
		PixelFormat getPixelFormat() const { return m_format; }

		virtual void update(const void* data, size_t size, unsigned int width, unsigned int height, PixelFormat format) = 0;
		
	protected:
		Texture& operator=(const Texture&) = delete;
		Texture(const Texture&) = delete;

		void setPixelFormat(PixelFormat fmt) { m_format = fmt; }	

	protected:
		RenderDevice& m_owner;
		PixelFormat m_format;
		ImageFormat m_image_format;
		Size m_size;
		std::string m_filename;
		std::string m_resourceGroup;
	};

	typedef std::shared_ptr<Texture> TexturePtr;
}