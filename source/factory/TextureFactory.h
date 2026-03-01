#pragma once
#include "AEEngine.h"
#include "vector"

namespace TextureFactory
{
	enum CardTextureID : s8{
		C_DISPLAY = 0, C_SAMPLE = 1, C_SLASH = 2, C_SLASH2 = 3, C_BLACKHOLE = 4, C_BARRIER = 5,
		C_BARRIER2 = 6
	};
	class TextureFactory
	{
		private:
		std::vector<AEGfxTexture*> card;
		std::vector<AEGfxTexture*> floor;
		std::vector<AEGfxTexture*> character;
		s8 fontID;

		public:
		void textureInit();
		TextureFactory();
		~TextureFactory();
		AEGfxTexture* getTextureCard(CardTextureID id);
		AEGfxTexture* getTextureFloor(s8 id);
		AEGfxTexture* getTextureChar(s8 id);
		s8 getFontID();
		void addTextureCard(AEGfxTexture* pTex);
		void addTextureFloor(AEGfxTexture* pTex);
		void addTextureChar(AEGfxTexture* pTex);
		
	};
}