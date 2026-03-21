#include "pch.h"
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
		for (AEGfxTexture* pTex : this->ui)
		{
			AEGfxTextureUnload(pTex);
		}
		for (AEGfxTexture* pTex : this->others)
		{
			AEGfxTextureUnload(pTex);
		}
	}

	void TextureFactory::textureInit(){
		//CARDS
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/cardSample.png"));	//0
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/cardDisplay.png"));	//1
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_slash.png"));	//2
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_slash+.png"));	//3
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_barrier.png"));	//4
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_barrier+.png"));	//5
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_firebolt.png"));	//6
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_gust_of_wind.png"));	//7
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_shoot.png"));	//8
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_shoot+.png"));	//9
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_black_hole.png"));
		//this->addTextureCard(AEGfxTextureLoad("../../Assets/cards/card_aura_farm.png"));
		for (Entity card : card_system.cards_vec)
		{
			std::string tex = "../../Assets/cards/";
			std::string loc = ecs.getComponent<Components::card_image>(card)->location;
			tex += loc;
			if (card_texture_map.find(loc) == card_texture_map.end())
			{
				AEGfxTexture * ptex = AEGfxTextureLoad(tex.c_str());
				card_texture_map[loc] = ptex;
				this->addTextureCard(ptex);
			}
		}
		

		//FLOOR
		this->addTextureFloor(AEGfxTextureLoad("../../Assets/floor/floor4.png"));

		//CHAR
		this->addTextureChar(AEGfxTextureLoad("../../Assets/character/enemy1.png"));
		this->addTextureChar(AEGfxTextureLoad("../../Assets/character/enemy2.png"));
		this->addTextureChar(AEGfxTextureLoad("../../Assets/character/Hero_sprite_sheet.png"));

		//UI

		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/binButton.png"));         //0
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/deckButton.png"));        //1
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/endturnButton.png"));     //2
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/manaEmpty.png"));         //3
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/manaholderElement.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/manaOne.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/manaTwo.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/skillWindow.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/turnCounter.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/pauseButton.png"));
		this->addTextureUI(AEGfxTextureLoad("../../Assets/UI/moveButton.png"));

		//Others
		this->addTextureOthers(AEGfxTextureLoad("../../Assets/others/Rock.png"));
		this->addTextureOthers(AEGfxTextureLoad("../../Assets/others/Combat.png"));
		this->addTextureOthers(AEGfxTextureLoad("../../Assets/others/Encounter.png"));
		//MIS
		//this->fontID = AEGfxCreateFont("../../Assets/font/cool.ttf", 64.F);
		this->fontID = AEGfxCreateFont("../../Assets/font/TahomaBd.ttf", 64.F);
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
	AEGfxTexture* TextureFactory::getTextureUI(s8 id)
	{
		return this->ui.at(id);
	}
	AEGfxTexture* TextureFactory::getTextureOthers(s8 id)
	{
		return this->others.at(id);
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
	void TextureFactory::addTextureUI(AEGfxTexture* pTex)
	{
		this->ui.push_back(pTex);
	}
	void TextureFactory::addTextureOthers(AEGfxTexture* pTex)
	{
		this->others.push_back(pTex);
	}
	AEGfxTexture* TextureFactory::getTextureFromCardMap(std::string key)
	{
		return this->card_texture_map[key];
	}
	s8 TextureFactory::getFontID(){
		return this->fontID;
	}
}