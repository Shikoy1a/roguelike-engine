#pragma once
#include <vector>
#include <string>
#include <utility>

// 路径：一串 (x, y) 坐标
using Path = std::vector<std::pair<int,int>>;

// A* 寻路：从 (sx, sy) 到 (tx, ty)
// 如果找不到路径，返回空的 Path
Path find_path(const std::vector<std::string>& map,
               int sx, int sy,
               int tx, int ty);
