#include "pathfinding.hpp"
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm> 

namespace {
    // 把 (x, y) 映射到一维下标
    int index(int x, int y, int width) {
        return y * width + x;
    }

    bool in_bounds(const std::vector<std::string>& map, int x, int y) {
        int h = static_cast<int>(map.size());
        int w = static_cast<int>(map[0].size());
        return x >= 0 && x < w && y >= 0 && y < h;
    }

    bool is_walkable(const std::vector<std::string>& map, int x, int y) {
        if (!in_bounds(map, x, y)) return false;
        char tile = map[y][x];
        return tile == '.';
    }

    int manhattan(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    struct Node {
        int idx;
        int f; // f = g + h
    };

    struct CompareNode {
        bool operator()(const Node& a, const Node& b) const {
            return a.f > b.f; // 小 f 优先
        }
    };
}

// A* 实现
Path find_path(const std::vector<std::string>& map,
               int startX, int startY,
               int goalX, int goalY)
{
    int h = static_cast<int>(map.size());
    if (h == 0) return {};
    int w = static_cast<int>(map[0].size());
    int total = w * h;

    int startIdx = index(startX, startY, w);
    int goalIdx  = index(goalX, goalY, w);

    const int INF = std::numeric_limits<int>::max();

    std::vector<int> gScore(total, INF);
    std::vector<int> fScore(total, INF);
    std::vector<int> cameFrom(total, -1);
    std::vector<bool> inOpen(total, false);
    std::vector<bool> closed(total, false);

    auto hCost = [&](int idx) {
        int x = idx % w;
        int y = idx / w;
        return manhattan(x, y, goalX, goalY);
    };

    std::priority_queue<Node, std::vector<Node>, CompareNode> openSet;

    gScore[startIdx] = 0;
    fScore[startIdx] = hCost(startIdx);
    openSet.push({startIdx, fScore[startIdx]});
    inOpen[startIdx] = true;

    const int dirs[4][2] = {
        { 1, 0 },
        {-1, 0 },
        { 0, 1 },
        { 0,-1 }
    };

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        int curIdx = current.idx;

        if (closed[curIdx]) continue;
        closed[curIdx] = true;

        if (curIdx == goalIdx) {
            // 重建路径
            Path path;
            int idx = goalIdx;
            while (idx != -1 && idx != startIdx) {
                int x = idx % w;
                int y = idx / w;
                path.push_back({x, y});
                idx = cameFrom[idx];
            }
            // 现在 path 是从 goal 到 start 的反向，反转一下
            std::reverse(path.begin(), path.end());
            return path;
        }

        int curX = curIdx % w;
        int curY = curIdx / w;

        for (auto& d : dirs) {
            int nx = curX + d[0];
            int ny = curY + d[1];

            if (!is_walkable(map, nx, ny) && !(nx == goalX && ny == goalY)) {
                // 目标格子可以允许是玩家所在位置，所以单独放行 goal
                continue;
            }

            int nIdx = index(nx, ny, w);
            if (closed[nIdx]) continue;

            int tentativeG = gScore[curIdx] + 1; // 每步代价为 1

            if (tentativeG < gScore[nIdx]) {
                cameFrom[nIdx] = curIdx;
                gScore[nIdx] = tentativeG;
                fScore[nIdx] = tentativeG + hCost(nIdx);

                if (!inOpen[nIdx]) {
                    inOpen[nIdx] = true;
                    openSet.push({nIdx, fScore[nIdx]});
                }
            }
        }
    }

    // 没找到路径
    return {};
}
