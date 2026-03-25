#include "pch.h"
#include "CardResolver.h"
#include "../system/GridSystem.h"
#include <algorithm>
namespace
{
	int get_category(int id)
	{
		return id / 1000;
	}

	int get_family(int id)
	{
		return (id / 100) % 10;
	}

	int get_targetting_code(int id)
	{
		return (id / 10) % 10;
	}

	int get_variant(int id)
	{
		return id % 10;
	}
}

namespace CardResolver
{
	PC_RETURN_TAG resolve(
		EntityComponent::Registry& ecs,
		CombatNameSpace::CombatSystem& combatSystem,
		Grid::GameBoard& board,
		TBS::TurnBasedSystem& tbs,
		Entity caster,
		Entity cardID,
		Entity target,
		AEVec2 targetPos)
	{
		(void)board;
		(void)targetPos;

		if (cardID == Components::NULL_INDEX)
			return PC_RETURN_TAG::INVALID;

		Components::Card_ID* idComp = ecs.getComponent<Components::Card_ID>(cardID);
		Components::Card_Value* value = ecs.getComponent<Components::Card_Value>(cardID);
		Components::Name* name = ecs.getComponent<Components::Name>(cardID);

		if (!idComp || !value || !name)
			return PC_RETURN_TAG::INVALID;

		int serialID = idComp->value;

		int category = get_category(serialID);
		int family = get_family(serialID);
		int targettingCode = get_targetting_code(serialID);
		int variant = get_variant(serialID);

		std::cout << "[CardResolver] Resolving card: " << name->value
			<< " | ID: " << serialID
			<< " | category: " << category
			<< " | family: " << family
			<< " | target: " << targettingCode
			<< " | variant: " << variant << std::endl;

		switch (category)
		{
			//-----------------------------------
			// 1XXX = ATTACK
			//-----------------------------------
		case 1:
		{
			combatSystem.play_attack_card(ecs, caster, cardID, target, targetPos);
			return PC_RETURN_TAG::VALID;
		}

		//-----------------------------------
		// 2XXX = DEFENSE
		//-----------------------------------
		case 2:
		{
			Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
			if (!stats)
				return PC_RETURN_TAG::INVALID;

			switch (family)
			{
			case 0: // Shield
				stats->shields += static_cast<int>(value->value);

				// cap at 30
				if (stats->shields > 30)
					stats->shields = 30;

				return PC_RETURN_TAG::VALID;

			case 1: // Aura Farm
				if (serialID == 2100) // DMG Cut
				{
					stats->damageTakenMultiplier = 0.5f;
					std::cout << "[CardResolver] DMG Cut applied.\n";
					return PC_RETURN_TAG::VALID;
				}
				else if (serialID == 2101) // Aura Farm
				{
					stats->damageTakenMultiplier = 0.0f;
					std::cout << "[CardResolver] Aura Farm applied.\n";
					return PC_RETURN_TAG::VALID;
				}
				break;

			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		//-----------------------------------
		// 3XXX = ITEM
		//-----------------------------------
		case 3:
		{
			switch (family)
			{
			case 0: // Healing
			{
				Components::HP* hp = ecs.getComponent<Components::HP>(caster);
				if (!hp)
					return PC_RETURN_TAG::INVALID;

				hp->c_value += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case 1: // PP buff
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->points += static_cast<int>(value->value);
				return PC_RETURN_TAG::VALID;
			}

			case 2: // Movement buff
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->cur_movSpd += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case 3: // ATK buff
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				const int maxStacks = 3;
				const int buffHits = 3;
				const f32 buffStep = 0.5f;

				// First activation: start the hit counter
				if (stats->atkBuffHitsLeft <= 0)
				{
					stats->atkBuffHitsLeft = buffHits;
					stats->atkBuffStacks = 1;
					stats->atkMultiplier = 1.0f + buffStep; // 1.5x
				}
				else
				{
					// Active buff: stack only up to maxStacks, do not refresh hits
					if (stats->atkBuffStacks < maxStacks)
					{
						++stats->atkBuffStacks;
						stats->atkMultiplier += buffStep;
					}
				}

				std::cout << "[CardResolver] ATK buff applied. Multiplier = "
					<< stats->atkMultiplier
					<< " | Hits Left = "
					<< stats->atkBuffHitsLeft
					<< " | Stacks = "
					<< stats->atkBuffStacks << '\n';

				return PC_RETURN_TAG::VALID;
			}

			case 4: // Draw / generate family
			{
				int count = static_cast<int>(value->value);
				for (int i = 0; i < count; ++i)
				{
					tbs.DrawPhase_add_card(ecs);
				}
				return PC_RETURN_TAG::VALID;
			}

			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		//-----------------------------------
		// 4XXX = EVENT
		//-----------------------------------
		case 4:
		{
			switch (family)
			{
			case 0: // Absorb / pull
			{
				std::vector<Entity> movedEnemies;
				std::cout << "[CardResolver] Absorb event triggered\n";

				int cx = static_cast<int>(targetPos.x);
				int cy = static_cast<int>(targetPos.y);

				Components::Targetting_Component* tc =
					ecs.getComponent<Components::Targetting_Component>(cardID);
				if (!tc)
					return PC_RETURN_TAG::INVALID;

				int aoe = static_cast<int>(tc->aoe);

				EntityComponent::ComponentTypeID tagID =
					EntityComponent::getComponentTypeID<Components::Tag>();

				bool anyMoved = false;

				std::vector<std::pair<int, Entity>> targets; // distance, entity

				for (int i = 0; i < MAX_I; ++i)
				{
					for (int j = 0; j < MAX_J; ++j)
					{
						if (grid_dist_manhattan(i, j, cx, cy) > aoe)
							continue;

						Entity e = board.get_pos()[i][j];
						if (e == Components::NULL_INDEX) continue;
						if (e == caster) continue;

						if (!ecs.getBitMask()[e].test(tagID)) continue;

						Components::Tag* tag = ecs.getComponent<Components::Tag>(e);
						if (!tag) continue;
						if (*tag == Components::Tag::OTHERS) continue;

						int dist = grid_dist_manhattan(i, j, cx, cy);
						targets.push_back({ dist, e });
					}
				}

				std::sort(targets.begin(), targets.end(),
					[](const std::pair<int, Entity>& a, const std::pair<int, Entity>& b)
					{
						return a.first > b.first; // farthest first
					});

				for (const auto& t : targets)
				{
					Entity e = t.second;
					if (board.moveEntityAI(e, cx, cy, 5))
					{
						anyMoved = true;
						movedEnemies.push_back(e);
					}
				}

				if (!movedEnemies.empty())
				{
					int randomIndex = std::rand() % movedEnemies.size();
					Entity doomed = movedEnemies[randomIndex];

					int roll = std::rand() % 100;

					if (roll < 10)
					{
						s32 ex, ey;
						if (board.findEntityCell(doomed, ex, ey))
						{
							combatSystem.get_graveyard().push_back({ AEVec2{ (f32)ex, (f32)ey }, doomed });
							std::cout << "[Black Hole] Enemy " << doomed << " was instantly eliminated.\n";
						}
					}
					else
					{
						Call_AttackSystem(caster, 40.0f, board);
						std::cout << "[Black Hole] Failed instant kill. Player took 40 damage.\n";
					}
				}

				if (anyMoved)
				{
					board.setEnemyAnimationPhase();
				}

				return PC_RETURN_TAG::VALID;
			}
			case 1: // Push
			{
				std::cout << "[CardResolver] Push event triggered\n";

				// 0 = east, 1 = south, 2 = west, 3 = north
				int dx = 0;
				int dy = 0;

				switch (board.placementDirection)
				{
				case 0: dx = 1;  dy = 0;  break;
				case 1: dx = 0;  dy = 1;  break;
				case 2: dx = -1; dy = 0;  break;
				case 3: dx = 0;  dy = -1; break;
				default: break;
				}

				std::vector<Entity> targets;

				EntityComponent::ComponentTypeID tagID =
					EntityComponent::getComponentTypeID<Components::Tag>();
				EntityComponent::ComponentTypeID gdID =
					EntityComponent::getComponentTypeID<Components::gridData>();
				EntityComponent::ComponentTypeID astarID =
					EntityComponent::getComponentTypeID<Components::AStarResult>();
				EntityComponent::ComponentTypeID animID =
					EntityComponent::getComponentTypeID<Components::Animation_Actor>();

				// collect all valid pushed targets first
				for (int i = 0; i < MAX_I; ++i)
				{
					for (int j = 0; j < MAX_J; ++j)
					{
						Entity e = board.get_pos()[i][j];
						if (e == Components::NULL_INDEX) continue;
						if (e == caster) continue;

						if (!ecs.getBitMask()[e].test(tagID)) continue;
						if (!ecs.getBitMask()[e].test(gdID)) continue;
						if (!ecs.getBitMask()[e].test(astarID)) continue;
						if (!ecs.getBitMask()[e].test(animID)) continue;

						Components::Tag* tag = ecs.getComponent<Components::Tag>(e);
						if (!tag) continue;

						// skip grid objects like Mana Wall
						if (*tag == Components::Tag::OTHERS) continue;

						targets.push_back(e);
					}
				}

				bool anyMoved = false;

				// push each target up to 5 tiles by queueing a movement path
				for (Entity e : targets)
				{
					Components::gridData* gd = ecs.getComponent<Components::gridData>(e);
					Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(e);
					Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(e);

					if (!gd || !astar || !anim) continue;

					int startX = gd->x;
					int startY = gd->y;

					std::list<Components::GridCell> pushPath;

					int finalX = startX;
					int finalY = startY;

					// temporarily free the start cell so the enemy can move out of it
					board.get_pos()[startX][startY] = Components::NULL_INDEX;
					board.walkable[startY * MAX_I + startX] = 1;

					for (int step = 1; step <= 5; ++step)
					{
						int nx = startX + dx * step;
						int ny = startY + dy * step;

						// stop at board edge for now
						if (nx < 0 || nx >= MAX_I || ny < 0 || ny >= MAX_J)
							break;

						// stop if blocked by something already occupying the tile
						if (board.get_pos()[nx][ny] != Components::NULL_INDEX)
							break;

						pushPath.push_back({ nx, ny });
						finalX = nx;
						finalY = ny;
					}

					// no movement: restore original board state
					if (pushPath.empty())
					{
						board.get_pos()[startX][startY] = e;
						board.walkable[startY * MAX_I + startX] = 0;
						continue;
					}

					// reserve final board position immediately
					board.get_pos()[finalX][finalY] = e;
					board.walkable[finalY * MAX_I + finalX] = 0;

					// queue animation path
					astar->path = pushPath;
					anim->setType(Components::AnimationType::ENEMY_MOVING);

					// update logical grid position to final reserved tile
					gd->prev_x = startX;
					gd->prev_y = startY;
					gd->x = finalX;
					gd->y = finalY;

					anyMoved = true;
				}

				if (anyMoved)
				{
					board.setEnemyAnimationPhase();
				}

				return PC_RETURN_TAG::VALID;
			}

			case 2: // Mana Wall
			{
				std::cout << "[CardResolver] Mana Wall triggered\n";
				// spawn wall here
				return PC_RETURN_TAG::VALID;
			}
			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		default:
			return PC_RETURN_TAG::INVALID;
		}
	}

	//overload for invoving placing entity (Mana wall)
	PC_RETURN_TAG resolve(
		EntityComponent::Registry& ecs,
		CombatNameSpace::CombatSystem& combatSystem,
		Grid::GameBoard& board,
		TBS::TurnBasedSystem& tbs,
		MeshFactory& mf,
		TextureFactory::TextureFactory& TF,
		Entity caster,
		Entity cardID,
		Entity target,
		AEVec2 targetPos)
	{
		if (cardID == Components::NULL_INDEX)
			return PC_RETURN_TAG::INVALID;

		Components::Card_ID* idComp = ecs.getComponent<Components::Card_ID>(cardID);
		if (!idComp)
			return PC_RETURN_TAG::INVALID;

		int serialID = idComp->value;
		int category = get_category(serialID);
		int family = get_family(serialID);

		// Mana Wall here
		if (category == 4 && family == 2)   // e.g. 4200
		{
			std::cout << "[CardResolver] Mana Wall triggered\n";

			int cx = static_cast<int>(targetPos.x);
			int cy = static_cast<int>(targetPos.y);
			bool manaWallVertical = (board.placementDirection % 2);

			int offsets[3][2];

			if (!manaWallVertical)
			{
				offsets[0][0] = -1; offsets[0][1] = 0;
				offsets[1][0] = 0; offsets[1][1] = 0;
				offsets[2][0] = 1; offsets[2][1] = 0;
			}
			else
			{
				offsets[0][0] = 0; offsets[0][1] = -1;
				offsets[1][0] = 0; offsets[1][1] = 0;
				offsets[2][0] = 0; offsets[2][1] = 1;
			}

			// validate first
			for (int i = 0; i < 3; ++i)
			{
				int x = cx + offsets[i][0];
				int y = cy + offsets[i][1];

				if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
					return PC_RETURN_TAG::INVALID;

				if (board.get_pos()[x][y] != Components::NULL_INDEX)
					return PC_RETURN_TAG::INVALID;
			}

			// spawn wall pieces
			for (int i = 0; i < 3; ++i)
			{
				int x = cx + offsets[i][0];
				int y = cy + offsets[i][1];

				Entity wall = EntityFactory::create_grid_object(
					ecs,
					mf,
					{ 0.0f, 0.0f },
					{ 238.0f, 238.0f },
					"ManaWall",
					TF.getTextureOthers(5), 
					1.0f
				);

				Components::Transform* trans = ecs.getComponent<Components::Transform>(wall);
				if (trans && manaWallVertical)
				{
					trans->size.x = -248.0f;
					trans->size.y = 248.0f;
				}

				board.placeEntity(wall, x, y);
			}

			return PC_RETURN_TAG::VALID;
		}

		// all normal cards use the old resolver
		return resolve(ecs, combatSystem, board, tbs, caster, cardID, target, targetPos);
	}
}