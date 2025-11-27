#include "pathfinding.hpp"
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm> // std::reverse

// 节点索引：把 (x, y) 映射到一个 int，方便存 map
static int toIndex(int x, int y, int width) {
    return y * width + x;
}

// 从索引还原成 (x, y)
static std::pair<int,int> fromIndex(int idx, int width) {
    return { idx % width, idx / width };
}

// 判断地图格子是否可通行（只看地图，不看实体）
// 你可以根据自己的定义调整，这里假设 '.' 是可走的
static bool is_walkable_tile_map_only(const std::vector<std::string>& map,
                                      int x, int y) {
    int height = static_cast<int>(map.size());
    int width  = static_cast<int>(map[0].size());
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    return map[y][x] == '.';
}

Path find_path(const std::vector<std::string>& map,
               int sx, int sy,
               int tx, int ty) {
    Path empty;

    int height = static_cast<int>(map.size());
    if (height == 0) return empty;
    int width  = static_cast<int>(map[0].size());

    // 起点或终点本身不可走，直接无路可走
    if (!is_walkable_tile_map_only(map, sx, sy) &&
        !(sx == tx && sy == ty)) {
        return empty;
    }
    if (!is_walkable_tile_map_only(map, tx, ty)) {
        return empty;
    }

    struct Node {
        int idx;
        int f; // f = g + h
    };

    auto heuristic = [=](int x, int y) {
        // 曼哈顿距离
        return std::abs(x - tx) + std::abs(y - ty);
    };

    // open list：用最小堆按 f 排序
    struct NodeCmp {
        bool operator()(const Node& a, const Node& b) const {
            return a.f > b.f; // 小顶堆
        }
    };

    std::priority_queue<Node, std::vector<Node>, NodeCmp> open;
    std::unordered_map<int, int> cameFrom;         // child idx -> parent idx
    std::unordered_map<int, int> gScore;           // idx -> g
    const int INF = std::numeric_limits<int>::max();

    int startIdx = toIndex(sx, sy, width);
    int goalIdx  = toIndex(tx, ty, width);

    gScore[startIdx] = 0;
    open.push({ startIdx, heuristic(sx, sy) });

    auto get_g = [&](int idx) {
        auto it = gScore.find(idx);
        if (it == gScore.end()) return INF;
        return it->second;
    };

    std::vector<bool> closed(width * height, false);

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (closed[current.idx]) continue;
        closed[current.idx] = true;

        if (current.idx == goalIdx) {
            // 找到目标，回溯路径
            Path path;
            int idx = goalIdx;
            while (true) {
                auto [cx, cy] = fromIndex(idx, width);
                path.push_back({cx, cy});
                if (idx == startIdx) break;
                idx = cameFrom[idx];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        auto [cx, cy] = fromIndex(current.idx, width);

        // 4 方向邻居
        const int dirs[4][2] = {
            { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
        };

        for (auto& d : dirs) {
            int nx = cx + d[0];
            int ny = cy + d[1];

            if (!is_walkable_tile_map_only(map, nx, ny) &&
                !(nx == tx && ny == ty)) {
                continue;
            }

            int nIdx = toIndex(nx, ny, width);
            if (closed[nIdx]) continue;

            int tentativeG = get_g(current.idx);
            if (tentativeG == INF) continue;
            tentativeG += 1; // 每步代价 1

            if (tentativeG < get_g(nIdx)) {
                gScore[nIdx]   = tentativeG;
                cameFrom[nIdx] = current.idx;
                int f = tentativeG + heuristic(nx, ny);
                open.push({ nIdx, f });
            }
        }
    }

    // 找不到路径
    return empty;
}
