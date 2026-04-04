#pragma once

#include "../types.h"

#include "AEEngine.h"
#include "vector"


namespace TextureFactory
{
	enum CardTextureID : s8{
		C_DISPLAY = 0, C_SAMPLE = 1, C_SLASH = 2, C_SLASH2 = 3,  C_BARRIER = 4,
		C_BARRIER2 = 5, C_FIREBOLT = 6, C_GUSTOFWIND = 7, C_SHOOT = 8, C_SHOOT2 = 9,
		C_BLACKHOLE , C_AURAFARM
	};
	class TextureFactory
	{
		private:
		std::vector<AEGfxTexture*> card;
		std::vector<AEGfxTexture*> floor;
		std::vector<AEGfxTexture*> character;
		std::vector<AEGfxTexture*> ui;
		std::vector<AEGfxTexture*> others;
		std::vector<AEGfxTexture*> tutorial;

		std::unordered_map<std::string, AEGfxTexture*> card_texture_map;
		std::unordered_map<std::string, AEGfxTexture*> enemy_texture_map;

		s8 fontID;

		public:
		void textureInit();
		TextureFactory();
		~TextureFactory();
		AEGfxTexture* getTextureCard(CardTextureID id);
		AEGfxTexture* getTextureFloor(s8 id);
		AEGfxTexture* getTextureChar(s8 id);
		AEGfxTexture* getTextureUI(s8 id);
		AEGfxTexture* getTextureOthers(s8 id);
		AEGfxTexture* getTextureTutorial(s8 id);
		s8 getFontID();
		void addTextureCard(AEGfxTexture* pTex);
		void addTextureFloor(AEGfxTexture* pTex);
		void addTextureChar(AEGfxTexture* pTex);
		void addTextureUI(AEGfxTexture* pTex);
		void addTextureOthers(AEGfxTexture* pTex);
		void addTextureTutorial(AEGfxTexture* pTex);

		AEGfxTexture* getTextureFromCardMap(std::string key);
		AEGfxTexture* getTextureFromEnemyMap(std::string key);
	};
}