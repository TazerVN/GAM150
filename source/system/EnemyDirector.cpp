#include "pch.h"
#include "EnemyDirector.h"
#include "../system/CombatSystem.h" // for attack
#include "../util/Pathfinding.h"    // for A*

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <cstdlib>   // std::stoi
#include <cmath>     // std::abs

// remove leading white spaces
// normalises the lines read from the script
static std::string ltrim_copy(std::string s)
{
    size_t i = 0;
    while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
    s.erase(0, i);
    return s;
}

// decides what lines to ignore
bool EnemyDirector::isCommentOrEmpty(const std::string& line)
{
    std::string s = ltrim_copy(line); // trim first

    // Remove UTF-8 BOM (or it will read some weird chras liek \xEF\xFNE0)
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
    {
        s.erase(0, 3);
        s = ltrim_copy(s);
    }
    // Comments labelled with '#' in the script
    if (s.empty()) return true;
    return s[0] == '#';
}

// converts lines into tokens
// stored in a vector
EnemyDirector::Tokens EnemyDirector::tokenize(const std::string& line)
{
    std::istringstream iss(line);
    Tokens out;
    std::string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

bool EnemyDirector::loadScriptFile(const std::string& filePath)
{
    std::cout << "[EnemyDirector] Trying to open: " << filePath << "\n";

    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        std::cout << "[EnemyDirector] Failed to open: " << filePath << "\n";
        return false;
    }

    std::cout << "[EnemyDirector] Opened OK.\n";

    timeline_.clear();
    timelineIp_ = 0;
    spawnCount_ = 0;

    enum class Section
    {
        NONE,
        SPAWN,
        BEHAVIOUR
    };

    Section section = Section::NONE;

    std::string line;
    int loaded = 0;

    while (std::getline(ifs, line))
    {
        std::cout << "[ED] raw line: '" << line << "'\n";

        std::string s = ltrim_copy(line);

        // handle empty
        if (s.empty()) continue;

        // section headers
        if (s == "# ---- SPAWN ----")
        {
            section = Section::SPAWN;
            continue;
        }
        if (s == "# -- BEHVAIOUR --" || s == "# -- BEHAVIOUR --")
        {
            section = Section::BEHAVIOUR;
            continue;
        }

        // skip comments
        if (isCommentOrEmpty(s)) continue;

        if (section == Section::SPAWN)
        {
            spawnCount_ = std::stoi(s);
            std::cout << "[ED] spawnCount = " << spawnCount_ << "\n";
            continue;
        }

        if (section == Section::BEHAVIOUR)
        {
            Tokens t = tokenize(s);
            std::cout << "[ED] tokens=" << t.size() << "\n";
            if (t.empty()) continue;

            timeline_.push_back(std::move(t));
            ++loaded;
        }
    }

    std::cout << "[ED] timeline loaded lines: " << timeline_.size() << "\n";
    std::cout << "[ED] spawn count: " << spawnCount_ << "\n";
    std::cout << "[EnemyDirector] Loaded " << loaded << " behaviour lines.\n";

    return spawnCount_ > 0 || loaded > 0;
}

int EnemyDirector::getSpawnCount() const
{
    return spawnCount_;
}

void EnemyDirector::bindActor(const std::string& actorId, Entity e)
{
    idToEntity_[actorId] = e;
    entityToId_[e] = actorId;

    std::cout << "[ED] bindActor " << actorId << " -> entity " << e << "\n";
}


void EnemyDirector::update(PhaseSystem::GameBoardState& gbs,
    TBS::TurnBasedSystem& tbs,
    Grid::GameBoard& board,
    IntentionDisplaySystem& intent)
{
    (void)gbs;

    if (timeline_.empty()) return;

    Entity cur = tbs.current();
    if (cur == Components::NULL_INDEX) return;

    // If it is player's turn, reset latch and do nothing
    if (cur == playerID)
    {
        hordeTurnActive_ = false;
        return;
    }

    if(gbs.getEnemyPhase() == PhaseSystem::EnemyPhase::ENEMY_ANIMATION)
    {
        return;
    }

    // Once enemy turn starts, keep it latched until STOP is reached
    if (!hordeTurnActive_)
    {
        hordeTurnActive_ = true;
        gbs.set_GBPhase(PhaseSystem::GBPhase::ENEMY_PHASE);
        gbs.debug_print();
        std::cout << "\n[ED] ===== HORDE TIMELINE BEGIN =====\n";
    }

    if (timelineIp_ >= timeline_.size())
        timelineIp_ = 0;

    const Tokens& cmd = timeline_[timelineIp_];

    std::cout << "[ED] cmd: ";
    for (auto const& s : cmd) std::cout << s << ' ';
    std::cout << "\n";

    // STOP ends this horde chunk and advances turn
    if (cmd.size() == 1 && cmd[0] == "STOP")
    {
        ++timelineIp_;
        tbs.next();
        intent.trigger();
        std::cout << "[ED] STOP hit. End of horde chunk.\n";
        gbs.debug_print();
        return;
    }

    // malformed line
    if (cmd.size() < 2)
    {
        std::cout << "[ED] malformed line, skipping.\n";
        ++timelineIp_;
        return;
    }

    const std::string& actorId = cmd[0];

    auto it = idToEntity_.find(actorId);
    if (it == idToEntity_.end())
    {
        std::cout << "[ED] no bound entity for actorId=" << actorId << "\n";
        ++timelineIp_;
        return;
    }

    Entity actor = it->second;

    if (cmd[1] == "MOVE")
    {
        execMOVE(board, actor, playerID, cmd);
    }
    else if (cmd[1] == "ATTACK")
    {
        execATTACK(board, gbs ,actor, playerID, cmd);
    }
    else
    {
        std::cout << "[ED] unknown verb: " << cmd[1] << "\n";
    }

    ++timelineIp_;
}

const std::vector<EnemyDirector::Tokens>& EnemyDirector::get_timeline() const
{
    return timeline_;
}

std::unordered_map<std::string, Entity>& EnemyDirector::get_map()
{
    return idToEntity_;
}

void EnemyDirector::print_current_instruction() const
{
    std::cout << "\nCurrent Instruction VV" << std::endl;
    size_t index = timelineIp_;
    bool brake = true;
    while (brake)
    {
        if (index >= timeline_.size()) index = 0;
        if (timeline_[index][0] == "STOP")
            brake = false;
        for (std::string a : timeline_[index++])
        {
            std::cout << a << ' ';
        }
    }
}

size_t EnemyDirector::index() const
{
    return timelineIp_;
}
void EnemyDirector::execMOVE(Grid::GameBoard& board,
    Entity actor,
    Entity playerID,
    const Tokens& t)
{
    // Supported CMDs:
    // E0 MOVE FRONT
    // E1 MOVE IN_RANGE PLAYER 2
    if (t.size() < 3) return;

    s32 ax, ay, px, py;
    if (!board.findEntityCell(actor, ax, ay)) return;
    if (!board.findEntityCell(playerID, px, py)) return;

    auto& pos = board.get_pos();

    auto inBounds = [](int x, int y) {
        return x >= 0 && x < MAX_I && y >= 0 && y < MAX_J;
        };

    const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

    auto tryStep = [&](int nx, int ny) {
        if (!inBounds(nx, ny)) {
            std::cout << "[ED] step OOB -> " << nx << "," << ny << "\n";
            return false;
        }
        if (pos[nx][ny] != -1) {
            std::cout << "[ED] step BLOCKED by entity " << pos[nx][ny]
                << " at " << nx << "," << ny << "\n";
            return false;
        }
        bool ok = board.moveEntityAI(actor, nx, ny);
        std::cout << "[ED] step " << (ok ? "OK" : "FAILED") << " -> " << nx << "," << ny << "\n";
        return ok;
        };

    auto manhattan = [](int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
        };

    //
    auto pickAdjacentToPlayer = [&]() -> std::pair<int, int>
        {
            int bestX = -1, bestY = -1;
            int bestD = 1e9;

            for (auto& d : dirs)
            {
                int x = px + d[0], y = py + d[1];
                if (!inBounds(x, y)) continue;
                if (pos[x][y] != -1) continue;

                int dist = manhattan(ax, ay, x, y);
                if (dist < bestD) { bestD = dist; bestX = x; bestY = y; }
            }
            return { bestX, bestY };
        };

    if (t[2] == "FRONT")
    {
        int steps = 1;
        if (t.size() >= 4)
        {
            try
            {
                steps = std::stoi(t[3]);
                if (steps < 1) steps = 1;
            }
            catch (...)
            {
                steps = 1;
            }
        }

        // refresh positions
        if (!board.findEntityCell(actor, ax, ay)) return;
        if (!board.findEntityCell(playerID, px, py)) return;

        // find the best empty tile adjacent to player
        auto target = pickAdjacentToPlayer();
        int tx = target.first;
        int ty = target.second;

        if (tx < 0)
        {
            std::cout << "[ED] FRONT no adjacent empty tile near player.\n";
            return; // player surrounded
        }

        // already standing at the chosen front tile
        if (ax == tx && ay == ty)
        {
            std::cout << "[ED] FRONT already in best front position.\n";
            return;
        }

        //// build walkable grid for A*
        //std::vector<uint8_t> walkable(MAX_I * MAX_J, 1);

        //for (int y = 0; y < MAX_J; ++y)
        //{
        //    for (int x = 0; x < MAX_I; ++x)
        //    {
        //        Entity ent = pos[x][y];
        //        if (ent != Components::NULL_INDEX && ent != actor)
        //        {
        //            walkable[y * MAX_I + x] = 0;
        //        }
        //    }
        //}

        Components::GridCell start{ ax, ay };
        Components::GridCell goal{ tx, ty };

        board.moveEntityAI(actor, tx, ty);

        /*Components::AStarResult result =
            AStar_FindPath_Grid4(MAX_I, MAX_J, walkable.data(), start, goal);

        if (result.path.empty())
        {
            std::cout << "[ED] FRONT no path found.\n";
            return;
        }*/

        //// path includes start cell, so next move begins at index 1
        //int maxMoves = std::min<int>(steps, static_cast<int>(result.path.size()) - 1);

        //auto itPath = result.path.begin();
        //++itPath; // skip start cell

        //for (int moved = 0; moved < maxMoves && itPath != result.path.end(); ++moved, ++itPath)
        //{
        //    int nx = itPath->x;
        //    int ny = itPath->y;

        //    bool ok = board.moveEntityAI(actor, nx, ny);
        //    if (!ok)
        //    {
        //        std::cout << "[ED] FRONT moveEntityAI failed at "
        //            << nx << "," << ny << "\n";
        //        return;
        //    }
        //}

        //std::cout << "[ED] FRONT path moved " << maxMoves << " step(s).\n";
        return;
    }

    // IN_RANGE PLAYER R: if dist <= R do nothing; else step closer
    if (t[2] == "IN_RANGE" && t.size() >= 5 && t[3] == "PLAYER")
    {
        int R = std::stoi(t[4]);
        int distNow = manhattan(ax, ay, px, py);
        if (distNow <= R) return;

        int bestNx = ax, bestNy = ay;
        int bestDist = distNow;

        for (auto& d : dirs)
        {
            int nx = ax + d[0], ny = ay + d[1];
            if (!inBounds(nx, ny)) continue;
            if (pos[nx][ny] != -1) continue;

            int nd = manhattan(nx, ny, px, py);
            if (nd < bestDist) { bestDist = nd; bestNx = nx; bestNy = ny; }
        }

        if (bestNx != ax || bestNy != ay) {
            bool ok = board.moveEntityAI(actor, bestNx, bestNy);
            std::cout << "[ED] BACK step " << (ok ? "OK" : "FAIL")
                << " -> " << bestNx << "," << bestNy << "\n";
        }
        else {
            std::cout << "[ED] BACK no further tile (blocked)\n";
        }
        return;
    }

    if (t[2] == "RIGHT")
    {
        tryStep(ax + 1, ay);
        return;
    }

    if (t[2] == "LEFT")
    {
        tryStep(ax - 1, ay);
        return;
    }

    if (t[2] == "UP")
    {
        tryStep(ax, ay - 1);
        return;
    }

    if (t[2] == "DOWN")
    {
        tryStep(ax, ay + 1);
        return;
    }

    // Disengage
    if (t[2] == "BACK")
    {
        int bestNx = ax, bestNy = ay;
        int bestDist = manhattan(ax, ay, px, py);

        for (auto& d : dirs)
        {
            int nx = ax + d[0], ny = ay + d[1];
            if (!inBounds(nx, ny)) continue;
            if (pos[nx][ny] != -1) continue;

            int nd = manhattan(nx, ny, px, py);
            if (nd > bestDist) { bestDist = nd; bestNx = nx; bestNy = ny; }
        }

        if (bestNx != ax || bestNy != ay)
            board.moveEntityAI(actor, bestNx, bestNy);

        return;
    }

    std::cout << "[EnemyDirector] MOVE args not recognized.\n";
}

void EnemyDirector::execATTACK(Grid::GameBoard& board,
                               PhaseSystem::GameBoardState& gbs,
    Entity actor,
    Entity playerID,
    const Tokens& t)
{
    if (t.size() < 3)
    {
        std::cout << "[ED] ATTACK missing damage value.\n";
        return;
    }

    s32 ax, ay, px, py;
    if (!board.findEntityCell(actor, ax, ay))
    {
        std::cout << "[ED] ATTACK could not find actor on board.\n";
        return;
    }

    if (!board.findEntityCell(playerID, px, py))
    {
        std::cout << "[ED] ATTACK could not find player on board.\n";
        return;
    }

    int damage = 0;
    try
    {
        damage = std::stoi(t[2]);
    }
    catch (...)
    {
        std::cout << "[ED] ATTACK invalid damage value: " << t[2] << "\n";
        return;
    }

    auto manhattan = [](int x1, int y1, int x2, int y2)
        {
            return std::abs(x1 - x2) + std::abs(y1 - y2);
        };

    int dist = manhattan(ax, ay, px, py);
    const int attackRange = 2;

    if (dist <= attackRange)
    {
        auto aa = ecs.getComponent<Components::Animation_Actor>(actor);

        aa->anim_type = Components::AnimationType::ENEMY_ATTACK;
        gbs.set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_ANIMATION);

        COMBAT_SYSTEM_RETURN_TAG result =
            Call_AttackSystem(playerID, static_cast<f32>(damage),board);

        if (result == COMBAT_SYSTEM_RETURN_TAG::VALID)
        {
            std::cout << "[ED] ATTACK success: actor " << actor
                << " dealt " << damage
                << " to player at range " << dist << "\n";
        }
        else
        {
            std::cout << "[ED] ATTACK failed: player cannot take damage.\n";
        }
    }
    else
    {
        std::cout << "[ED] ATTACK skipped: player out of range. Dist="
            << dist << " Range=" << attackRange << "\n";
    }
}