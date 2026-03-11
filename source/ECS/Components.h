#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H


#include "AEEngine.h"
#include <vector>
#include <string>
#include <functional>

#include "../global.h"
#include "../factory/MeshFactory.h"
#include "../system/CardConstants.h"

#define MAX_CARDS 1000
#define MAX_COMPONENT 20

constexpr size_t NULL_INDEX = -1;
constexpr size_t DRAW_COUNT = 6;

namespace Components
{
	enum class Tag{
		CARDS, GRID, ACTOR, UI
	};

	struct TagClass{
		Tag value;
	};

	struct Text{
		const char* text;
		s8 fontID;
		s8 z;
		Text(const char*, s8, s8);
	};


	struct RGBA
	{
		f32 r;
		f32 g;
		f32 b;
		f32 a;
		RGBA(f32 r, f32 g, f32 b, f32 a);
	};

	struct Timer{
		f32 seconds;
		f32 max_seconds;
		bool start;
		bool reset;
		Timer();
		Timer(f32 max, f32 current = 0, bool start = true, bool reset = false);
	};

	struct GridCell{
		s32 x,y;
	};

	struct Switch
	{
		bool s;
	};

	/*struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEVec2 size_col{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};*/
	struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEVec2 size_col{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};

	struct Texture
	{
		AEGfxTexture* texture{ nullptr };
		f32 offset_x, offset_y;
		Texture(AEGfxTexture* texture, f32 offset_x = 0, f32 offset_y = 0);
	};

	struct Mesh
	{
		bool on;
		AEGfxVertexList* mesh = nullptr;
		RENDER_MODE r_mode;
		MESH_TYPE type;
		s8 z;
	};

	struct Color
	{
		RGBA c_color;
		RGBA d_color;
		Color(f32 r, f32 g, f32 b, f32 a);
	};

	struct Input
	{
		public:
		bool on;
		u8 type; //for AE internal input system check
		bool hover;
		bool drag;
		bool col;
		s8 z;
		Input(u8 type, bool hover, 
			  std::function<void()> onClick, 
			  std::function<void()> onHover, 
			  std::function<void()> offHover,
			  s8 z = 0, bool drag = false, bool col = true);
		std::function<void()> onClick;
		std::function<void()> onHover;
		std::function<void()> offHover;
	};

	struct Particle
	{
		AEVec2	velocity;
		f32		fadeSpeed;
		bool	alive;
	};


	//===========================CARDS=========================================


	/*struct Card_Value
	{
		f32 value = 0.0f;
		CardType type;
	};*/
	struct Card_Value
	{
		f32 value=0.0f;
		CardType type;
	};


	/*struct Targetting_Component
	{
		Targetting targetting_type;
		f32 range = 0.0f;
		f32 aoe = 0.0f;
	};*/
	struct Targetting_Component
	{
		Targetting targetting_type;
		f32 range=0.0f;
		f32 aoe=0.0f;
	};


	struct Card_Cost
	{
		f32 value;
	};

	struct Attack
	{
		f32 damage;
		CardType type;
		f32 range;
	};

	struct Name
	{
		std::string value;
	};

	struct HP
	{
		f32 c_value;
		f32 max_value;
		HP() = default;
		HP(f32 value);
	};
	class Card_Storage
	{
		public:
		void add_card_to_deck(Entity cardID);
		void add_card_to_hand(Entity cardID);
		void remove_card_from_hand(int index);
		void reshuffle_discard2deck();

		std::vector<size_t> data_deck;
		std::vector<size_t> data_card_hand;
		std::vector<size_t> data_discard_pile;
	};

	//struct TurnBasedStats
	//	int maxPoints;			// per-round cap (set when entity created)
	//	int points {0};			// current round points (reset at round start)
	//	int shields{0};
	//	f32 ini_movSpd;
	//	f32 max_movSpd{ini_movSpd}, cur_movSpd{ max_movSpd };
	struct TurnBasedStats
	{
		int maxPoints;			// per-round cap (set when entity created)
		int points {0};			// current round points (reset at round start)
		int shields{0};
		f32 ini_movSpd;
		f32 max_movSpd{ini_movSpd}, cur_movSpd{ max_movSpd };
	};

}
#endif