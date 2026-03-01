#include "TextureFactory.h"
namespace TextureFactory
{
	TextureFactory::TextureFactory()
	{
		card.clear();
		character.clear();
		floor.clear();
		this->fontID = -1;
	}
	TextureFactory::~TextureFactory()
	{
		for(AEGfxTexture* pTex: this->card){
			AEGfxTextureUnload(pTex);
		}
		for (AEGfxTexture* pTex : this->character)
		{
			AEGfxTextureUnload(pTex);
		}
		for (AEGfxTexture* pTex : this->floor)
		{
			AEGfxTextureUnload(pTex);
		}
	}

	void TextureFactory::textureInit(){
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/cardSample.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/cardDisplay.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_slash.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_slash+.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_black_hole.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_barrier.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_barrier+.png"));
		this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_aura_farm.png"));


		this->addTextureFloor(AEGfxTextureLoad("../../Assets/floor/floor4.png"));

		this->addTextureChar(AEGfxTextureLoad("../../Assets/character/enemy1.png"));
		this->addTextureChar(AEGfxTextureLoad("../../Assets/character/enemy2.png"));
		this->fontID = AEGfxCreateFont("../../Assets/font/cool.ttf", 64.F);
	}


	AEGfxTexture* TextureFactory::getTextureCard(CardTextureID id)
	{
		return this->card.at(id);
	}
	AEGfxTexture* TextureFactory::getTextureFloor(s8 id)
	{
		return this->floor.at(id);
	}
	AEGfxTexture* TextureFactory::getTextureChar(s8 id)
	{
		return this->character.at(id);
	}

	void TextureFactory::addTextureCard(AEGfxTexture* pTex)
	{
		this->card.push_back(pTex);
	}
	void TextureFactory::addTextureFloor(AEGfxTexture* pTex)
	{
		this->floor.push_back(pTex);
	}
	void TextureFactory::addTextureChar(AEGfxTexture* pTex)
	{
		this->character.push_back(pTex);
	}

	s8 TextureFactory::getFontID(){
		return this->fontID;
	}
}