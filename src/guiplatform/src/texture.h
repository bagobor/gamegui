#pragma once

#include <guilib/src/texture.h>

namespace gui
{
namespace rgde_platform
{
	class renderer;

	class texture : public Texture
	{
	public:	
		texture(renderer& owner, rgde::render::texture_ptr tex);
		virtual ~texture();
		
		rgde::render::texture_ptr get_platform_resource() const {return m_texture;}

		virtual	void onDeviceLost(void);
		virtual	void onDeviceReset(void);

	private:
		void free_platform_resource(void);
		virtual void calculateMetrics();

	private:
		rgde::render::texture_ptr m_texture;
		std::string m_filename;
		std::string m_resourceGroup;
	};
}
}