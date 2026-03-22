#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../ECS/ECSystem.h"
#include "../system/PhaseSystem.h"
#include "../system/TurnBasedSystem.h"
#include "../system/GridSystem.h"   // GameBoard (Grid::GameBoard)
#include "../UI/IntentionDisplay.h"


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
    // Execute one horde chunk until a STOP line is reached
    void update(PhaseSystem::GameBoardState& gbs,
        TBS::TurnBasedSystem& tbs,
        Grid::GameBoard& board,
        IntentionDisplaySystem& intent);

    // Number of enemies to spawn for this level
    int getSpawnCount() const;

private:
    // Number of enemies listed in the SPAWN section
    int spawnCount_ = 0;

    // Global behaviour timeline read in exact file order
    std::vector<Tokens> timeline_;
    size_t timelineIp_ = 0;
    bool hordeTurnActive_ = 0;

    // Actor ID binding stuff
    std::unordered_map<std::string, Entity> idToEntity_;        // "E0" -> entity
    std::unordered_map<Entity, std::string> entityToId_;        // entity -> "E0"

private:
    static bool isCommentOrEmpty(const std::string& line);
    static Tokens tokenize(const std::string& line);

    // ============== Command executors =============

    // Move Entity
    void execMOVE(Grid::GameBoard& board,
        Entity actor,
        Entity playerID,
        const Tokens& t);

    // Entity Attacks
    void execATTACK(Grid::GameBoard& board,
        Entity actor,
        Entity playerID,
        const Tokens& t);

    // More to come!!!
    // Cast soon >>>>> 

public:
    const std::vector<Tokens>& get_timeline() const;
    std::unordered_map<std::string, Entity>& get_map(); // "E0" -> entity
    void print_current_instruction() const;
    size_t index() const;
};