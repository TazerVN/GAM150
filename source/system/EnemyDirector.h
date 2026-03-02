#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../ECS/ECSystem.h"
#include "../system/PhaseSystem.h"
#include "../system/TurnBasedSystem.h"
#include "../system/GridSystem.h"   // GameBoard (Grid::GameBoard)

// Loads lines like: "E0 MOVE FRONT" and executes one line per enemy turn.
// refer to /Assets/Level/TEST_level.txt for script structure
class EnemyDirector
{
public:
    using Tokens = std::vector<std::string>; // Tokens stored as a Vector

    // Load a script file
    bool loadScriptFile(const std::string& filePath);

    // Bind script actor IDs to actual ECS entities (e.g. Entity 6 -> E0)
    void bindActor(const std::string& actorId, Entity e);

    // Called once per frame in Scene::update
    // exceute at most 1 instruciton per enemy turn/phase
    void update(ECS::Registry& ecs,
        PhaseSystem::GameBoardState& gbs,
        TBS::TurnBasedSystem& tbs,
        Grid::GameBoard& board,
        Entity playerID);

private:
    // Assign instructions and ID's to entities as per the script
    struct ScriptState {
        std::vector<Tokens> lines;
        size_t ip = 0; // instruction pointer
    };

    std::unordered_map<std::string, ScriptState> scripts_;      // "E0" -> lines
    std::unordered_map<std::string, Entity> idToEntity_;        // "E0" -> entity
    std::unordered_map<Entity, std::string> entityToId_;        // entity -> "E0"

    // latch to prevent executing every frame for same current enemy
    Entity lastActorExecuted_ = NULL_INDEX;

private:
    static bool isCommentOrEmpty(const std::string& line);
    static Tokens tokenize(const std::string& line);

    bool getActorIdFromEntity(Entity e, std::string& outId) const;
    bool hordeTurnConsumed_ = false;

    size_t lastRoundSeen_ = 0;
    Entity lastCurSeen_ = NULL_INDEX;

    struct QueuedAction
    {
        Entity actor = NULL_INDEX;
        std::string actorId;
        Tokens cmd;          // tokens should look like ["E0","MOVE","LEFT","STOP"]
        bool stopAfter = false;
        bool valid = false;
    };

    QueuedAction queued_;
    bool hordeActive_ = false;     // horde's turn ?
    bool yieldToPlayer_ = false;   // for STOP token; pass to player when token is STOP

    // ============== Command executors =============

    // Move Entity
    void execMOVE(ECS::Registry& ecs,
        Grid::GameBoard& board,
        Entity actor,
        Entity playerID,
        const Tokens& t);

    // More to come!!!
};