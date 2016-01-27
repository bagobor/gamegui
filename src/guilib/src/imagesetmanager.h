#pragma once

#include "rect.h"
#include "Size.h"
#include <memory>
#include <unordered_map>

namespace xml
{
	class node;
};

namespace gui
{
	class System;
	class Imageset;
	class Texture;
	typedef std::shared_ptr<Texture> TexturePtr;

	struct SubImage
	{
		SubImage() : m_ordinal(-1), m_offset(0, 0) {}
		size_t m_ordinal; // ordinal to texture in atlas
		Rect m_src; // area in atlas
		Rect m_crop; // for a cropped images, offset, and original size
		point m_offset; // offset in result image
	};

	//struct RenderImageInfo;
	struct RenderImageInfo
	{
		RenderImageInfo() : texture(nullptr), isAdditiveBlend(false) {}
		Texture* texture;
		Rect pixel_rect;
		point offset;
		point crop;
		bool  isAdditiveBlend;
	};

	class Image
	{
	public:
		typedef std::vector<SubImage> SubImages;

		/// @brief - Image with regions, if data.size() != 0
		/// @param parent - Ordinals in the sub-images will point to textures in this imageset
		/// @param sz - full image size
		/// @param data - sub-images
		Image(Imageset* parent, const std::string& name, const Size& sz, SubImages& data, bool isAdditiveBlend = false);
		/// @brief - An empty image
		Image();

		const Size& size() const;
		Imageset* parent() const;
		/// @brief - returns count of subimages
		size_t count() const;
		/// @brief - render info for an image
		/// @param info - output
		/// @param subimage - in range [0, GetCount())
		void GetRenderInfo(RenderImageInfo& info, size_t subimage) const;
		
		const std::string& name() const;

		void setAdditiveBlend(bool is_addivie) { m_isAdditiveBlend = is_addivie; }
		bool getAdditiveBlend()  const { return m_isAdditiveBlend; }

	private:
		Size m_size;
		SubImages m_data;
		Imageset* m_parent;
		std::string m_name;
		bool m_isAdditiveBlend;
	};

	inline const Size& Image::size() const
	{
		return m_size;
	}

	inline Imageset* Image::parent() const
	{
		return m_parent;
	}

	inline size_t Image::count() const
	{
		return m_data.size();
	}

	inline const std::string& Image::name() const
	{
		return m_name;
	}

	/// sample XML:
	//<Imageset Name="CoolImageset!">
	//	<Textures>
	//		<Texture Name="UberTex" Filename="test.tga"/>
	//		<Texture Filename="test1.tga"/>
	//	</Textures>
	//	<Images>
	//		<Image Name="test img" Width="100" Height="200" >
	//			<Rect Texture="UberTex" SrcLeft="0" SrcTop="0" SrcRight="100" SrcBottom="100" XPos="0" YPos="0" />
	//			<Rect Texture="test1.tga" SrcLeft="0" SrcTop="0" SrcRight="100" SrcBottom="100" XPos="0" YPos="100"
	//				CropLeft="5" CropTop="5" OrigWidth="110" OrigHeight="110" />
	//		</Image>
	//	</Images>
	//</Imageset>

	class Imageset
	{
	public:
		/// @brief - General way to create and load imageset
		/// @param sys - System reference(load texture, log)
		/// @param name - debug only name
		/// @param imgset - pointer to xml node named "imageset", optional
		Imageset(System& sys, const std::string& name, xml::node* imgset = 0);

		Imageset(const std::string& name);

		/// @brief - New texture pushed back. No texture deletion support!
		/// @param tex - texture ptr
		/// @returns - ordinal to texture, for a SubImage
		size_t AppendTexture(TexturePtr tex);

		/// @brief - Suppose to be used in manual image creation
		/// @param name - image name, used for an access to image
		/// @param sz - full image size
		/// @param data - subimage data - texture coords, and size, output size() == 0 is success
		/// @returns - true if success
		bool DefineImage(const std::string& name, const Size& sz, Image::SubImages& data);

		/// @brief - Used to load from loaded xml
		/// @param imgset - xml node named "imageset"
		/// @param name - debug only name
		/// @returns - true if success
		bool Load(xml::node* imgset, const std::string& name, System& sys);

		/// @brief - loaded texture count
		size_t GetTextureCount() const;
		/// @brief - return the empty texture if ordinal is out of range
		TexturePtr GetTexture(size_t ordinal) const;

		const Image* GetImage(const std::string& name) const;
		const Image* operator[](const std::string& name) const { return GetImage(name); }
		const Image* GetImagePtr(std::string name) const { return GetImage(name); }
		/// @brief - Get image by index
		/// @param idx - [0, ImagesCount())
		/// @returns - ptr, 0 if out of range
		const Image* GetImageByIdx(size_t idx) const;

		const std::string& GetName() const;

		size_t ImagesCount() const;

	private:
		typedef std::vector<TexturePtr> Textures;
		Textures m_textures;
		typedef std::unordered_map<std::string, Image> Images;
		Images m_images;
		std::string m_name;
	};

	inline size_t Imageset::GetTextureCount() const
	{
		return m_textures.size();
	}

	inline size_t Imageset::ImagesCount() const
	{
		return m_images.size();
	}

	inline TexturePtr Imageset::GetTexture(size_t ordinal) const
	{
		return ordinal < m_textures.size() ? m_textures[ordinal] : TexturePtr();
	}

	inline const std::string& Imageset::GetName() const
	{
		return m_name;
	}

	typedef std::shared_ptr<Imageset> ImagesetPtr;
	typedef std::weak_ptr<Imageset> ImagesetWeakPtr;

	/// @brief - producing and loading imagesets
	class ImagesetManager
	{
	public:
		/// @brief - Produce an empty imageset using name
		/// @param name - must be unique, otherwise returned previous
		/// @returns - ptr
		ImagesetPtr MakeEmpty(System& sys, const std::string& name);
		/// @brief - Produse an imageset using loaded xml
		/// @param imgset - xml node named "imageset"
		ImagesetPtr Make(System& sys, xml::node* imgset);
	private:
		ImagesetPtr Produce(System& sys, const std::string& name, xml::node* imgset);

	private:
		typedef std::unordered_map<std::string, ImagesetWeakPtr> ImagesetRegistry;
		ImagesetRegistry m_registry;
	};
}