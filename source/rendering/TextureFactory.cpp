#include "TextureFactory.h"
namespace TextureFactory
{
	TextureFactory::TextureFactory()
	{
		data.clear();
	}
	TextureFactory::~TextureFactory()
	{
		for(AEGfxTexture* pTex: this->data){
			AEGfxTextureUnload(pTex);
		}
	}


	AEGfxTexture* TextureFactory::getTexture(s8 id)
	{
		return this->data.at(id);
	}

	void TextureFactory::addTexture(AEGfxTexture* pTex)
	{
		this->data.push_back(pTex);
	}

}