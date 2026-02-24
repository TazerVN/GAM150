#pragma once
#include "AEEngine.h"
#include "vector"

namespace TextureFactory
{
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
		AEGfxTexture* getTextureCard(s8 id);
		AEGfxTexture* getTextureFloor(s8 id);
		AEGfxTexture* getTextureChar(s8 id);
		s8 getFontID();
		void addTextureCard(AEGfxTexture* pTex);
		void addTextureFloor(AEGfxTexture* pTex);
		void addTextureChar(AEGfxTexture* pTex);
		
	};
}