#pragma once
#include "AEEngine.h"
#include "vector"

namespace TextureFactory
{
	class TextureFactory
	{
		private:
		std::vector<AEGfxTexture*> data;
		public:
		TextureFactory();
		~TextureFactory();
		AEGfxTexture* getTexture(s8 id);
		void addTexture(AEGfxTexture* pTex);
		
	};
}