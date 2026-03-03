#include "EnemyDirector.h"

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
    scripts_.clear();

    std::string line;
    int loaded = 0;

    while (std::getline(ifs, line))
    {
        std::cout << "[ED] raw line: '" << line << "'\n";
        if (isCommentOrEmpty(line)) continue;

        Tokens t = tokenize(line);
        std::cout << "[ED] tokens=" << t.size() << "\n";
        if (t.size() < 2) continue;

        const std::string& actorId = t[0];
        scripts_[actorId].lines.push_back(std::move(t));
        loaded++;
    }

    std::cout << "[ED] scripts loaded keys: ";
    for (auto& kv : scripts_) std::cout << kv.first << "(" << kv.second.lines.size() << ") ";
    std::cout << "\n";

    std::cout << "[EnemyDirector] Loaded " << loaded << " instruction lines.\n";
    return loaded > 0;
}

void EnemyDirector::bindActor(const std::string& actorId, Entity e)
{
    idToEntity_[actorId] = e;
    entityToId_[e] = actorId;

    // ensure entry exists
    (void)scripts_[actorId];

    std::cout << "[ED] bindActor " << actorId << " -> entity " << e << "\n";
}

bool EnemyDirector::getActorIdFromEntity(Entity e, std::string& outId) const
{
    auto it = entityToId_.find(e);
    if (it == entityToId_.end()) return false;
    outId = it->second;
    return true;
}

void EnemyDirector::update(ECS::Registry& ecs,
    PhaseSystem::GameBoardState& gbs,
    TBS::TurnBasedSystem& tbs,
    Grid::GameBoard& board,
    Entity playerID)
{
    // Enemy turn started
    hordeTurnConsumed_ = true;

    // infinite loop guard
    // stops the game from freezing for any bugs related to turn passing
    const size_t guardMax = 64;
    size_t guard = 0;

    // Process enemies in initiative order until we reach player
    while (tbs.current() != playerID && guard++ < guardMax)
    {
        Entity cur = tbs.current();
        if (cur == NULL_INDEX) return;

        std::cout << "\n[ED] ===== Horde step =====\n";

        // =============== SKIPPING CONDITIONS ==================
        // find actor id
        std::string actorId;
        if (!getActorIdFromEntity(cur, actorId))
        {
            std::cout << "[ED] NO actorId binding for entity " << cur << " (skipping)\n";
            tbs.next(ecs);
            continue;
        }

        // find script lines for that actor
        auto sit = scripts_.find(actorId);
        if (sit == scripts_.end() || sit->second.lines.empty())
        {
            std::cout << "[ED] No script lines for actorId=" << actorId << " (skipping)\n";
            tbs.next(ecs);
            continue;
        }
        //============= END OF SKIPPING CONDITIONS ===========

        ScriptState& ss = sit->second;

        std::cout << "[ED] Acting actorId=" << actorId
            << " ip=" << ss.ip
            << " total=" << ss.lines.size() << "\n";

        bool foundSTOP = false;
        size_t safety = 0;
        const size_t maxSteps = ss.lines.size();

        // Execute commands until STOP
        while (!foundSTOP && safety < maxSteps)
        {
            if (ss.ip >= ss.lines.size()) ss.ip = 0;

            const Tokens& cmd = ss.lines[ss.ip];

            std::cout << "[ED] cmd: ";
            for (auto const& s : cmd) std::cout << s << ' ';
            std::cout << "\n";

            if (cmd.size() >= 2 && cmd[1] == "MOVE")
                execMOVE(ecs, board, cur, playerID, cmd);
            else
                std::cout << "[EnemyDirector] Unknown verb: "
                << (cmd.size() >= 2 ? cmd[1] : "?") << "\n";

            for (const std::string& tok : cmd)
                if (tok == "STOP") { foundSTOP = true; break; }

            ss.ip++;
            safety++;
        }


        // guard in case the LOOP does not have a STOP, auto stops at last instruction
        if (!foundSTOP)
        {
            std::cout << "[EnemyDirector] WARNING: No STOP found for " << actorId
                << " within one script loop; forcing end.\n";
        }

        // advance to next participant
        tbs.next(ecs);
    }

    if (guard >= guardMax)
        std::cout << "[EnemyDirector] ERROR: guardMax hit (turn loop).\n";
}

void EnemyDirector::execMOVE(ECS::Registry& ecs,
    Grid::GameBoard& board,
    Entity actor,
    Entity playerID,
    const Tokens& t)
{
    // Supported:
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

    // Helper: pick an empty adjacent tile to the player (best for this enemy)
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

    // FRONT: move 1 step toward an empty tile adjacent to player
    if (t[2] == "FRONT")
    {
        auto target = pickAdjacentToPlayer();
        int tx = target.first;
        int ty = target.second;

        if (tx < 0) return; // player surrounded

        int bestNx = ax, bestNy = ay;
        int bestD = manhattan(ax, ay, tx, ty);

        for (auto& d : dirs)
        {
            int nx = ax + d[0], ny = ay + d[1];
            if (!inBounds(nx, ny)) continue;
            if (pos[nx][ny] != -1) continue;

            int nd = manhattan(nx, ny, tx, ty);
            if (nd < bestD) { bestD = nd; bestNx = nx; bestNy = ny; }
        }

        if (bestNx != ax || bestNy != ay)
            board.moveEntityAI(actor, bestNx, bestNy);

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

    // MOVE RIGHT: +x
    if (t[2] == "RIGHT")
    {
        tryStep(ax + 1, ay);
        return;
    }
    // MOVE LEFT: -x
    if (t[2] == "LEFT")
    {
        tryStep(ax - 1, ay);
        return;
    }
    // MOVE UP: -y (depending on your grid, flip if needed)
    if (t[2] == "UP")
    {
        tryStep(ax, ay - 1);
        return;
    }
    // MOVE DOWN: +y
    if (t[2] == "DOWN")
    {
        tryStep(ax, ay + 1);
        return;
    }

    // MOVE BACK: 1 step away from player
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