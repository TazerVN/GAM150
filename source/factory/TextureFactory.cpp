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
		for (Entity card : card_system.cards_vec)
		{
			std::string tex = "Assets/cards/";
			std::string loc = ecs.getComponent<Components::image_location>(card)->location;
			tex += loc;
			if (card_texture_map.find(loc) == card_texture_map.end())
			{
				AEGfxTexture * ptex = AEGfxTextureLoad(tex.c_str());
				card_texture_map[loc] = ptex;
				this->addTextureCard(ptex);
			}
		}
		

		//FLOOR
		this->addTextureFloor(AEGfxTextureLoad("Assets/floor/floor4.png"));

		//CHAR
		this->addTextureChar(AEGfxTextureLoad("Assets/character/Hero_sprite_sheet_new.png"));	//first must always be hero or creator actor spritesheet will fail
		//add textures for enemies
		for (Entity enemy : beastiary.enemies_vec)
		{
			std::string tex = "Assets/character/";
			Components::image_location* i = ecs.getComponent<Components::image_location>(enemy);
			std::string loc = i->location;
			tex += loc;
			if (enemy_texture_map.find(loc) == enemy_texture_map.end())
			{
				AEGfxTexture* ptex = AEGfxTextureLoad(tex.c_str());
				enemy_texture_map[loc] = ptex;
				this->addTextureChar(ptex);
			}
		}

		//UI

		this->addTextureUI(AEGfxTextureLoad("Assets/UI/binButton.png"));         //0
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/deckButton.png"));        //1
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/endturnButton.png"));     //2
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/manaEmpty.png"));         //3
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/manaholderElement.png"));	//4
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/manaOne.png"));	//5
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/manaTwo.png"));	//6
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/skillWindow.png"));	//7
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/turnCounter.png"));	//8
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/pauseButton.png"));	//9
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/moveButton.png"));	//10

		//Others
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Rock.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Combat.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Encounter.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Enemy_atk_icon.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Enemy_move_icon.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Manawall.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Barrier.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Barrier+.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Gradient.png"));

		//MIS
		//this->fontID = AEGfxCreateFont("Assets/font/cool.ttf", 64.F);
		this->fontID = AEGfxCreateFont("Assets/font/TahomaBd.ttf", 64.F);
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
	AEGfxTexture* TextureFactory::getTextureFromEnemyMap(std::string key)
	{
		return this->enemy_texture_map[key];
	}
	s8 TextureFactory::getFontID(){
		return this->fontID;
	}
}