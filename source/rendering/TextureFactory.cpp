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

	void TextureFactory::textureInit(){
		this->addTexture(AEGfxTextureLoad("../../Assets/cardSample.png"));
		this->addTexture(AEGfxTextureLoad("../../Assets/floor4.png"));
		this->addTexture(AEGfxTextureLoad("../../Assets/enemy1.png"));
		this->addTexture(AEGfxTextureLoad("../../Assets/enemy2.png"));
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