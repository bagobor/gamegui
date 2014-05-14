#include "stdafx.h"
#include "imagesetmanager.h"

#include "system.h"
#include "renderer.h"

namespace gui
{
	Image::Image(Imageset* parent, const std::string& name, const Size& sz, SubImages& data, bool isAdditiveBlend)
		: m_parent(parent)
		, m_size(sz)
		, m_name(name)
		, m_isAdditiveBlend(isAdditiveBlend)
	{
		m_data.swap(data);
	}

	Image::Image() : m_parent(0)
	{
	}

	void Image::GetRenderInfo(RenderImageInfo& info, size_t subimage) const
	{
		if(m_parent && subimage < m_data.size())
		{
			const SubImage& sb = m_data[subimage];
			info.texture = m_parent->GetTexture(sb.m_ordinal).get();
			info.pixel_rect = sb.m_src;
			info.offset = sb.m_offset;
			info.crop = sb.m_crop.getPosition();
			info.isAdditiveBlend = m_isAdditiveBlend;
		}
	}

	Imageset::Imageset(System& sys, const std::string& name, xml::node* imgset)
		: m_name(name)
	{
		Load(imgset, name, sys);
	}

	Imageset::Imageset(const std::string& name)
		: m_name(name)
	{
	}

	size_t Imageset::AppendTexture(TexturePtr tex)
	{
		Textures::iterator it = std::find(m_textures.begin(), m_textures.end(), tex);
		if(it != m_textures.end())
			return it - m_textures.begin();
		m_textures.push_back(tex);
		return m_textures.size() - 1;
	}

	bool Imageset::DefineImage(const std::string& name, const Size& sz, Image::SubImages& data)
	{
		Images::iterator it = m_images.find(name);
		if(it != m_images.end())
			return false;
		// TODO: optimize multiple copy
		Image img(this, name, sz, data);
		m_images.insert(std::make_pair(name, img));
		return true;
	}

	bool Imageset::Load(xml::node* imgset, const std::string& name, System& sys)
	{
		if(imgset)
		{
			xml::node texsnode = imgset->child("Textures");
			if(!texsnode)
			{
				sys.logEvent(log::warning, std::string("The imageset ")
					+ name + " doesn't have any texture information. Imageset is unaffected");
				return false;
			}

			xml::node imgsnode = imgset->child("Images");
			if(!imgsnode)
			{
				sys.logEvent(log::warning, std::string("The imageset ")
					+ name + " doesn't have any image information. Imageset is unaffected");
				return false;
			}

			typedef std::unordered_map<std::string, size_t> TextureOrdinals;
			TextureOrdinals textureOrdinals;

			xml::node texnode = texsnode.first_child();
			while(!texnode.empty())
			{
				std::string nodename(texnode.name());
				if(nodename == "Texture")
				{
					std::string texname = texnode["Name"].value();
					std::string filename = texnode["Filename"].value();
					if(!texname.size())
						texname = filename;
					TextureOrdinals::iterator it = textureOrdinals.find(texname);
					if(it == textureOrdinals.end())
					{
						TexturePtr tex = sys.getRenderer().createTexture("imageset/" + filename);
						if(tex)
						{
							m_textures.push_back(tex);
							textureOrdinals.insert(std::make_pair(texname, m_textures.size() - 1));
						}
						else
						{
							sys.logEvent(log::warning, std::string("The imageset ")
								+ name + " unable to load texture '" + texname + "', file '"
								+ filename + "'.");
						}
					}
				}
				texnode = texnode.next_sibling();
			}

			xml::node imgnode = imgsnode.first_child();
			while(!imgnode.empty())
			{
				std::string nodename(imgnode.name());
				if(nodename == "Image")
				{
					std::string imgname = imgnode["Name"].value();
					float width = imgnode["Width"].as_float();
					float height = imgnode["Height"].as_float();

					Image::SubImages subImages;

					xml::node rectnode = imgnode.first_child();
					while(!rectnode.empty())
					{
						if(std::string(rectnode.name()) == "Rect")
						{
							std::string texname = rectnode["Texture"].value();
							TextureOrdinals::iterator it = textureOrdinals.find(texname);
							if(it != textureOrdinals.end())
							{
								SubImage sub;
								sub.m_ordinal = it->second;

								float left = rectnode["SrcLeft"].as_float();
								float top = rectnode["SrcTop"].as_float();
								float right = rectnode["SrcRight"].as_float();
								float bottom = rectnode["SrcBottom"].as_float();

								sub.m_src = Rect(left, top, right, bottom);

								float y = rectnode["YPos"].as_float();
								float x = rectnode["XPos"].as_float();
								sub.m_offset = point(x, y);

								if(!rectnode["CropLeft"].empty())
								{
									float cropx = rectnode["CropLeft"].as_float();
									float cropy = rectnode["CropTop"].as_float();
									float w = rectnode["OrigWidth"].as_float();
									float h = rectnode["OrigHeight"].as_float();
									sub.m_crop = Rect(point(cropx, cropy), Size(w, h));
								}
								subImages.push_back(sub);
							}
							else
							{
								sys.logEvent(log::warning, std::string("The imageset ")
									+ name + " can't find texture '" + texname + "' for the image '"
									+ imgname + "'.");
							}
						}
						rectnode = rectnode.next_sibling();
					}
					if(!subImages.empty())
					{
						// TODO: optimize multiple copy
						m_images.insert(std::make_pair(imgname, Image(this, imgname, Size(width, height), subImages)));
					}
					else
					{
						sys.logEvent(log::warning, std::string("The imageset ")
							+ name + " can't find any rectangles for the image '"
							+ imgname + "'. Skipping this image...");
					}
				}
				imgnode = imgnode.next_sibling();
			}

			return true;
		}
		return false;
	}

	const Image* Imageset::GetImage(const std::string& name) const
	{
		Images::const_iterator it = m_images.find(name);
		if(it != m_images.end())
			return &it->second;
		return 0;
	}

	const Image* Imageset::GetImageByIdx(size_t idx) const
	{
		if(idx < ImagesCount())
		{
			Images::const_iterator it = m_images.begin();
			std::advance(it, idx);
			return &it->second;
		}
		return 0;
	}

	ImagesetPtr ImagesetManager::MakeEmpty(System& sys, const std::string& name)
	{
		return Produce(sys, name, 0);
	}

	ImagesetPtr ImagesetManager::Make(System& sys, xml::node* imgset)
	{
		if(imgset)
		{
			std::string name = (*imgset)["Name"].value();
			return Produce(sys, name, imgset);
		}
		return ImagesetPtr();
	}

	ImagesetPtr ImagesetManager::Produce(System& sys, const std::string& name, xml::node* imgset)
	{
		ImagesetPtr retval;
		if(name.size())
		{
			ImagesetRegistry::iterator it = m_registry.find(name);
			if(it == m_registry.end())
			{
				retval.reset(new Imageset(sys, name, imgset));
				ImagesetWeakPtr weak = retval;
				m_registry.insert(std::make_pair(name, weak));
				return retval;
			}

			ImagesetWeakPtr& weak = it->second;
			if(retval = weak.lock())
				return retval;
			retval.reset(new Imageset(sys, name, imgset));
			weak = retval;
		}
		return retval;
	}

	ImageOps StringToImageOps(const std::string& str)
	{
		if(str == "Tile" || str == "tile")
			return Tile;
		if(str == "Stretch" || str == "stretch")
			return Stretch;

		return Stretch;
	}

	std::string ImageOpsToString(ImageOps op)
	{
		switch(op)
		{
		case Tile:
			return "Tile";
			break;
		case Stretch:
		default:
			return "Stretch";
			break;
		}
		return "Stretch";
	}

}