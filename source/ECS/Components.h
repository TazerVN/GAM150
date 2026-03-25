#pragma once

#include "../types.h"
#include "AEEngine.h"
#include "../system/CardConstants.h"

#include <vector>
#include <string>
#include <functional>
#include <array>



namespace Components
{
	constexpr size_t MAX_CARDS = 1000;
	constexpr size_t MAX_COMPONENT = 50;


	constexpr size_t NULL_INDEX = -1;
	constexpr size_t DRAW_COUNT = 6;
	//========================= ENUM ==============================
	enum class Tag
	{
		CARDS, GRID, ACTOR, UI, BACKGROUND, FOREGROUND, OTHERS
	};


	enum class ParticleType
	{
		Digitalize,
		Burst,
		Click,
		Datastream,
		Reversestream,
		Noise,
		Heal,
		Shield,
		Damage,
		Dataflow,
		Databub
	};

	enum class AnimationType : char
	{
		ATTACK_MELEE, ATTACK_RANGE, MOVING, IDLE, TAKING_DAMAGE, ENEMY_ATTACK, ENEMY_MOVING, DEATH ,NONE, COUNT
	};

	enum class IntentionType : char
	{
		ATTACK,SEVER_ATTACK,MOVE
	};

	//========================= COMMON ==============================
	/*struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEVec2 size_og{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};*/
	struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEVec2 size_og{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};

	struct gridData
	{
		s32 prev_x{ -1 };
		s32 prev_y{ -1 };
		s32 x{ -1 };
		s32 y{ -1 };
	};

	struct TagClass
	{
		Tag value;
	};

	struct Text
	{
		std::string text;
		s8 fontID;
		s8 z;
		Text(std::string, s8, s8);
	};


	struct RGBA
	{
		f32 r;
		f32 g;
		f32 b;
		f32 a;
		RGBA(f32 r, f32 g, f32 b, f32 a);
	};

	struct Timer
	{
		f32 seconds;
		f32 max_seconds;
		bool start;
		bool reset;
		Timer();
		Timer(f32 max, f32 current = 0, bool start = true, bool reset = false);
	};

	struct GridCell
	{
		s32 x, y;
		bool operator==(const GridCell& o) const { return x == o.x && y == o.y; }
	};

	struct AStarResult
	{
		std::list<GridCell> path; // includes start and goal if found
	};


	struct Switch
	{
		bool s;
	};


	struct Texture
	{
		AEGfxTexture* texture{ nullptr };
		f32 offset_x, offset_y;
		Texture(AEGfxTexture* texture, f32 offset_x = 0, f32 offset_y = 0);
	};


	/*
	* Z buffer:
		0: true background
		1: grid
		2 - 15: on grid stuff
		20 - 30: UI
	*/
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
			  s8 z = 0, bool drag = false, bool col = true, std::function<void()> onDrag = nullptr, std::function<void()> offDrag = nullptr);
		std::function<void()> onClick;
		std::function<void()> onHover;
		std::function<void()> offHover;
		std::function<void()> onDrag;
		std::function<void()> offDrag;
	};

	struct Velocity
	{
		AEVec2 vel{ 0.f,0.f };
	};


	struct Particle
	{
		ParticleType type;
	};

	struct Animation_Actor
	{
		public:
		AnimationType anim_type;
		AnimationType default_type;
		AnimationType prev_type;
		int current_frame;
		int max_frame;
		bool finished;
		std::array<Entity, static_cast<size_t>(Components::AnimationType::COUNT)> timer_array;

		Animation_Actor(Components::AnimationType type, int current_frame = 0, int max_frame = 0);
		void setType(AnimationType at);
		AnimationType getCurrType();
		AnimationType getDefaultType();
	};

	enum class VictoryNodeTag
	{
		COMBAT,ENCOUNTER,BOSS
	};

	//===========================CARDS=========================================


	/*struct Card_Value
	{
		f32 value = 0.0f;
		CardType type;
	};*/
	struct Card_Value
	{
		f32 value = 0.0f;
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
		f32 range = 0.0f;
		f32 aoe = 0.0f;
	};


	struct Card_Cost
	{
		f32 value;
	};

	struct Card_Description
	{
		std::string val;
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
	struct image_location
	{
		std::string location;
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
		void add_card_to_deck(Entity cardID);	//add to original pile
		void add_card_to_hand(Entity cardID);	//add card from copied data draw pile to hand
		void remove_card_from_hand(int index);	//remove card from hand to discardpile
		void reshuffle_discard2deck();
		void init();
		void free();
		void reset();


		std::vector<size_t> original_draw_pile;
		std::vector<size_t> data_draw_pile;
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
		int points{ 0 };			// current round points (reset at round start)
		int shields{ 0 };
		int atkBuffHitsLeft{ 0 };	// hits left for atk buff
		int atkBuffStacks{ 0 };		// stacks left ofr atk buff MAX 3
		f32 atkMultiplier{ 1.0f };	// multiplier
		f32 damageTakenMultiplier{ 1.0f }; // 0 = invincible... MARK?!
		f32 ini_movSpd;
		f32 max_movSpd{ ini_movSpd }, cur_movSpd{ max_movSpd };
	};

	// Fetch ID values
	struct Card_ID
	{
		int value;
	};

	struct Horde_Tag
	{
		std::vector<Entity>goons;
		void remove_goon(Entity);
		bool alive() const;
	};

}
