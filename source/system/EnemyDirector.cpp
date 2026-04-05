//===============================================================
//	AUTHOR      Zejin Kendreich Dayap Chen

//	EMAIL       chen.z@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#include "EnemyDirector.h"
#include "../system/CombatSystem.h" // for attack
#include "../util/Pathfinding.h"    // for A*
#include "global.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <cstdlib>   // std::stoi
#include <cmath>     // std::abs

//ranger helper

auto blocksRangedShot = [](Entity occupant) -> bool
    {
        if (occupant == Components::NULL_INDEX)
            return false;

        Components::Tag* tag = ecs.getComponent<Components::Tag>(occupant);
        if (tag && *tag == Components::Tag::OTHERS)
            return true;   // rock / wall / placed object blocks

        return false;      // enemies do not block
    };

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

bool EnemyDirector::isRangedActor(const std::string& actorId) const
{
    if (actorId.size() < 2) return false;
    if (actorId[0] != 'E') return false;

    int idx = -1;

    try
    {
        idx = std::stoi(actorId.substr(1));
    }
    catch (...)
    {
        return false;
    }

    return idx >= (spawnCount_ - rangedSpawnCount_);
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
    rangedSpawnCount_ = 0;
    mapObjectCount = 0;

    enum class Section
    {
        NONE,
        MAP,
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
        if (s == "# ----- MAP -----")
        {
            section = Section::MAP;
            continue;
        }
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

        if (section == Section::MAP)
        {
            Tokens t = tokenize(s);
            if (t.empty()) continue;

            if (t.size() == 1)
            {
                mapObjectCount = std::stoi(t[0]);
                std::cout << "[ED] mapObjectCount = " << mapObjectCount << "\n";
                section = Section::NONE;
                continue;
            }
        }

        if (section == Section::SPAWN)
        {
            Tokens t = tokenize(s);
            if (t.empty()) continue;

            // first SPAWN line: total count
            if (t.size() == 1)
            {
                spawnCount_ = std::stoi(t[0]);
                std::cout << "[ED] spawnCount = " << spawnCount_ << "\n";
                continue;
            }

            // optional second SPAWN line: "2 RANGE"
            if (t.size() == 2 && t[1] == "RANGE")
            {
                rangedSpawnCount_ = std::stoi(t[0]);
                std::cout << "[ED] rangedSpawnCount = " << rangedSpawnCount_ << "\n";
                continue;
            }
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

int EnemyDirector::getRangedSpawnCount() const
{
    return rangedSpawnCount_;
}

int EnemyDirector::getMapObjectCount() const {
    return mapObjectCount;
};


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

        intent.trigger();

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
        execMOVE(board, actor, cmd);
    }
    else if (cmd[1] == "ATTACK")
    {
        execATTACK(board, gbs ,actor, cmd);
    }
    else
    {
         auto aa = ecs.getComponent<Components::Animation_Actor>(actor);
            aa->setType(Components::AnimationType::ACTION);
        std::cout << "[ED] unknown verb: " << cmd[1] << "\n";
    }

    ++timelineIp_;
}

void EnemyDirector::removeEnemy(Entity id)
{
    if (this->entityToId_.find(id) != this->entityToId_.end())
    {
        std::string EID = entityToId_[id];
        entityToId_.erase(id);
        if (this->idToEntity_.find(EID) != this->idToEntity_.end())
            idToEntity_.erase(EID);
    }
}

const std::vector<std::pair<int, int>>& EnemyDirector::getPendingCells() const
{
    return pendingAttack_.cells;
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
    std::cout << "\nCurrent Instruction VV" << '\n';
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

    auto hasClearShotToPlayer = [&](int fromX, int fromY) -> bool
        {
            bool sameRow = (fromY == py);
            bool sameCol = (fromX == px);

            if (!sameRow && !sameCol)
                return false;

            int dx = (px > fromX) ? 1 : (px < fromX ? -1 : 0);
            int dy = (py > fromY) ? 1 : (py < fromY ? -1 : 0);

            int cx = fromX + dx;
            int cy = fromY + dy;

            while (cx != px || cy != py)
            {
                Entity occupant = pos[cx][cy];
                if (blocksRangedShot(occupant))
                    return false;

                cx += dx;
                cy += dy;
            }

            return true;
        };
    //
    auto pickAdjacentToPlayer = [&]() -> std::pair<int, int>
        {
            int bestX = -1, bestY = -1;
            int bestD = (int)1e9;

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
        int steps = 5; // default fallback

        if (t.size() >= 4)
        {
            try
            {
                steps = std::stoi(t[3]);

                if (steps <= 0)
                    steps = 1; // safety clamp
            }
            catch (...)
            {
                std::cout << "[ED] FRONT invalid steps, using default.\n";
                steps = 5;
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


        Components::GridCell start{ ax, ay };
        Components::GridCell goal{ tx, ty };

        ;
        if (board.moveEntityAI(actor, tx, ty, steps) == 0)
        {
            PUT << 2;
            PUT.display(actor, "LOST??");
        }

        return;
        
    }

    // IN_RANGE PLAYER R: if dist <= R do nothing; else step closer
    if (t[2] == "IN_RANGE" && t.size() >= 4 && t[3] == "PLAYER")
    {
        int steps = 1;
        if (t.size() >= 5)
        {
            try
            {
                steps = std::stoi(t[4]);
                if (steps < 1) steps = 1;
            }
            catch (...)
            {
                steps = 1;
            }
        }

        if (!board.findEntityCell(actor, ax, ay)) return;
        if (!board.findEntityCell(playerID, px, py)) return;

        const int preferredDist = 3;
        const int minDist = 2;

        int curDist = manhattan(ax, ay, px, py);
        bool sameRow = (ay == py);
        bool sameCol = (ax == px);

        // only stay if already aligned, close enough, AND has clear shot
        if ((sameRow || sameCol) &&
            curDist >= minDist && curDist <= preferredDist &&
            hasClearShotToPlayer(ax, ay))
        {
            std::cout << "[ED] IN_RANGE already aligned and in desired range with clear shot.\n";
            return;
        }

        int bestX = -1;
        int bestY = -1;
        int bestScore = 1000000000;

        for (int y = 0; y < MAX_J; ++y)
        {
            for (int x = 0; x < MAX_I; ++x)
            {
                if (!inBounds(x, y)) continue;
                if (pos[x][y] != -1) continue;

                bool alignsWithPlayer = (y == py) || (x == px);
                if (!alignsWithPlayer) continue;

                int distToPlayer = manhattan(x, y, px, py);

                // only accept tiles that are 2 or 3 away from player
                if (distToPlayer < minDist || distToPlayer > preferredDist)
                    continue;

                if (!hasClearShotToPlayer(x, y))
                    continue;

                int distFromActor = manhattan(ax, ay, x, y);

                // prefer closest movement for the actor
                // slight preference for exactly 3 tiles away
                int score = distFromActor * 10 + std::abs(distToPlayer - preferredDist);

                if (score < bestScore)
                {
                    bestScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }

        if (bestX < 0)
        {
            std::cout << "[ED] IN_RANGE no valid aligned tile at desired distance.\n";
            return;
        }

        if (board.moveEntityAI(actor, bestX, bestY, steps) == 0) {
            PUT << 2;
            PUT.display(actor, "LOST??");
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
    const Tokens& t)
{
    // Make up mind first if not already planned for this actor
    if (pendingAttack_.actor != actor)
    {
        if (planATTACK(board, actor, t) == false){
            PUT << 2;
            PUT.display(actor, "MISS");

            auto aa = ecs.getComponent<Components::Animation_Actor>(actor);
            aa->setType(Components::AnimationType::ACTION);
            //gbs.set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_ANIMATION);

        }
    }

    if (!pendingAttack_.canAttack)
    {
        pendingAttack_ = PendingAttack{}; // clear
        return;
    }

    auto aa = ecs.getComponent<Components::Animation_Actor>(actor);
    aa->setType(Components::AnimationType::ENEMY_ATTACK);
    gbs.set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_ANIMATION);

    COMBAT_SYSTEM_RETURN_TAG result =
        Call_AttackSystem(pendingAttack_.target,
            static_cast<f32>(pendingAttack_.damage),
            board);

    if (result == COMBAT_SYSTEM_RETURN_TAG::VALID)
    {
        std::cout << "[ED] ATTACK success: actor " << actor
            << (pendingAttack_.isRanged ? " [RANGED]" : " [MELEE]")
            << " dealt " << pendingAttack_.damage << "\n";
    }
    else
    {
        std::cout << "[ED] ATTACK failed.\n";
        PUT << "I FUCKED UP";
    }

    pendingAttack_ = PendingAttack{}; // clear after execution
}

bool EnemyDirector::planATTACK(Grid::GameBoard& board,
    Entity actor,
    const Tokens& t)
{
    pendingAttack_ = PendingAttack{}; // reset

    if (t.size() < 3)
    {
        std::cout << "[ED] PLAN ATTACK missing damage value.\n";
        return false;
    }

    s32 ax, ay, px, py;
    if (!board.findEntityCell(actor, ax, ay))
    {
        std::cout << "[ED] PLAN ATTACK could not find actor.\n";
        return false;
    }
    if (!board.findEntityCell(playerID, px, py))
    {
        std::cout << "[ED] PLAN ATTACK could not find player.\n";
        return false;
    }

    int damage = 10; // default

    if (t.size() >= 3)
    {
        try
        {
            damage = std::stoi(t[2]);
            if (damage <= 0) damage = 1;
        }
        catch (...)
        {
            std::cout << "[ED] ATTACK invalid damage, using default.\n";
            damage = 10;
        }
    }
    else
    {
        std::cout << "[ED] ATTACK missing damage, using default.\n";
    }

    auto manhattan = [](int x1, int y1, int x2, int y2)
        {
            return std::abs(x1 - x2) + std::abs(y1 - y2);
        };

    int dist = manhattan(ax, ay, px, py);

    bool isRanged = false;
    auto it = entityToId_.find(actor);
    if (it != entityToId_.end())
        isRanged = isRangedActor(it->second);

    pendingAttack_.actor = actor;
    pendingAttack_.target = playerID;
    pendingAttack_.damage = damage;
    pendingAttack_.isRanged = isRanged;

    if (isRanged)
    {
        bool sameRow = (ay == py);
        bool sameCol = (ax == px);

        if ((sameRow || sameCol) && dist <= rangedRange)
        {
            int dx = (px > ax) ? 1 : (px < ax ? -1 : 0);
            int dy = (py > ay) ? 1 : (py < ay ? -1 : 0);

            bool blocked = false;
            int cx = ax + dx;
            int cy = ay + dy;

            // collect cells along the line
            while (cx != px || cy != py)
            {
                Entity occupant = board.get_pos()[cx][cy];
                if (blocksRangedShot(occupant))
                {
                    blocked = true;
                    break;
                }
                pendingAttack_.cells.push_back({ cx, cy });
                cx += dx;
                cy += dy;
            }

            if (!blocked)
            {
                pendingAttack_.cells.push_back({ px, py }); // include player cell
                pendingAttack_.canAttack = true;
            }
            else
            {
                pendingAttack_.cells.clear();
                std::cout << "[ED] PLAN RANGED blocked by obstacle.\n";
            }
        }
        else
        {
            std::cout << "[ED] PLAN RANGED not lined up or out of range.\n";
        }
    }
    else
    {
        
        if (dist <= meleeRange)
        {
            pendingAttack_.cells.push_back({ px, py });
            pendingAttack_.canAttack = true;
        }
        else
        {
            std::cout << "[ED] PLAN MELEE out of range. Dist=" << dist << "\n";
        }
    }

    std::cout << "[ED] PLAN ATTACK decided: canAttack=" << pendingAttack_.canAttack
        << " cells=" << pendingAttack_.cells.size() << "\n";

    return pendingAttack_.canAttack;
}