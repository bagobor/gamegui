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

	public:
		explicit TextureManager(Renderer& render);
		~TextureManager();	

		void	onDeviceReset();
		void	onDeviceLost();

		TexturePtr	createTexture(const std::string& filename);
		void		pushTexture(TexturePtr tex);

		void	cleanup();

	protected:
		Textures m_textures;
		Renderer& m_render;
	};
}