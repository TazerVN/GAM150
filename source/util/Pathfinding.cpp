#include "Pathfinding.h"
#include <queue>
#include <limits>
#include <cmath>

static inline int Id(int x, int y, int w) { return y * w + x; }

struct NodeRec
{
    int id;
    int f;
    int g;
};

struct NodeRecGreater
{
    bool operator()(const NodeRec& a, const NodeRec& b) const
    {
        return a.f > b.f; // min-heap by f
    }
};

static inline int HeuristicManhattan(Cell a, Cell b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

AStarResult AStar_FindPath_Grid4(
    int width, int height,
    const uint8_t* walkable,
    Cell start, Cell goal
)
{
    AStarResult out;

    auto inBounds = [&](int x, int y)
        {
            return x >= 0 && x < width && y >= 0 && y < height;
        };

    if (!inBounds(start.x, start.y) || !inBounds(goal.x, goal.y))
        return out;

    if (!walkable[Id(start.x, start.y, width)] || !walkable[Id(goal.x, goal.y, width)])
        return out;

    const int N = width * height;
    const int INF = std::numeric_limits<int>::max();

    std::vector<int> g(N, INF);
    std::vector<int> parent(N, -1);
    std::vector<uint8_t> closed(N, 0);

    std::priority_queue<NodeRec, std::vector<NodeRec>, NodeRecGreater> open;

    const int startId = Id(start.x, start.y, width);
    const int goalId = Id(goal.x, goal.y, width);

    g[startId] = 0;
    open.push({ startId, HeuristicManhattan(start, goal), 0 });

    const int dx[4] = { 1,-1, 0, 0 };
    const int dy[4] = { 0, 0, 1,-1 };

    while (!open.empty())
    {
        NodeRec cur = open.top();
        open.pop();

        if (closed[cur.id]) continue;
        closed[cur.id] = 1;

        if (cur.id == goalId) break;

        int cx = cur.id % width;
        int cy = cur.id / width;

        for (int k = 0; k < 4; ++k)
        {
            int nx = cx + dx[k];
            int ny = cy + dy[k];
            if (!inBounds(nx, ny)) continue;

            int nid = Id(nx, ny, width);
            if (!walkable[nid]) continue;
            if (closed[nid]) continue;

            // uniform cost (1 per move)
            int tentativeG = g[cur.id] + 1;
            if (tentativeG < g[nid])
            {
                g[nid] = tentativeG;
                parent[nid] = cur.id;

                Cell nc{ nx, ny };
                int f = tentativeG + HeuristicManhattan(nc, goal);
                open.push({ nid, f, tentativeG });
            }
        }
    }

    if (parent[goalId] == -1 && goalId != startId)
        return out; // no path

    // reconstruct
    for (int at = goalId; at != -1; at = parent[at])
    {
        out.path.push_back({ at % width, at / width });
        if (at == startId) break;
    }
    std::reverse(out.path.begin(), out.path.end());
    return out;
}
