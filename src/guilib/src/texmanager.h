#pragma once


namespace gui
{
	class Renderer;
	class Texture;
	typedef boost::weak_ptr<Texture> TextureWeakPtr;
	typedef boost::shared_ptr<Texture> TexturePtr;

	class  TextureManager
	{
		typedef boost::unordered_map<std::string, TextureWeakPtr> Textures;
		typedef Textures::iterator TexturesIter;
		friend Renderer;

	public:
		explicit TextureManager(Renderer& render);
		~TextureManager();	
		
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