#pragma once

#include <unordered_map>

namespace gui
{
	class Renderer;
	class Texture;
	typedef std::weak_ptr<Texture> TextureWeakPtr;
	typedef std::shared_ptr<Texture> TexturePtr;

	//TODO: use resource manager
	class TextureCache
	{
		typedef std::unordered_map<std::string, TextureWeakPtr> Textures;
		typedef Textures::iterator TexturesIter;
		friend Renderer;

	public:
		explicit TextureCache(Renderer& render);
		~TextureCache();	
		
		TexturePtr	createTexture(const std::string& filename);
		void		pushTexture(TexturePtr tex);

		void	cleanup();

	protected:
		Textures& getTextures() { return m_textures; }

	protected:
		Textures m_textures;
		Renderer& m_render;
	};
}