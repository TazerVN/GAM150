//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "TextureFactory.h"
#include "global.h"
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
		for (AEGfxTexture* pTex : this->tutorial)
		{
			AEGfxTextureUnload(pTex);
		}
		if(fontID != -1) AEGfxDestroyFont(this->fontID);
	}

	void TextureFactory::textureInit(){
		//CARDS
		for (Entity card_ : card_system.cards_vec)
		{
			std::string tex = "Assets/cards/";
			//bug found here when you die/lose.
			std::string loc = ecs.getComponent<Components::image_location>(card_)->location;
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
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/Button_1.png"));	//11
		this->addTextureUI(AEGfxTextureLoad("Assets/UI/Button_2.png"));	//12

		//Others
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Rock.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Portal.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Encounter.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Enemy_atk_icon.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Enemy_move_icon.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Manawall.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Barrier.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Barrier+.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Gradient.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Beyond_the_NeXus_title.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Game_Over.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/Enemy_loading_icon.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/logo.png"));
		this->addTextureOthers(AEGfxTextureLoad("Assets/others/team.png"));

		//Tutorial
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_1.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_2.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_3.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_4.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_5.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_6.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_7.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_8.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_9.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_10.png"));
		this->addTextureTutorial(AEGfxTextureLoad("Assets/Tutorial/Page_11.png"));
		this->fontID = AEGfxCreateFont("Assets/font/TahomaBd.ttf", 64);
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
	AEGfxTexture* TextureFactory::getTextureTutorial(s8 id)
	{
		return this->tutorial.at(id);
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
	void TextureFactory::addTextureTutorial(AEGfxTexture* pTex)
	{
		this->tutorial.push_back(pTex);
	}
	AEGfxTexture* TextureFactory::getTextureFromCardMap(std::string key)
	{
		/*if(this->card_texture_map.find(key) != this->card_texture_map.end())
			return this->card_texture_map[key];

		return nullptr;*/
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