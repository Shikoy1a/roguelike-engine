# Roguelike 命令行 & 图形游戏引擎（C++ / raylib）

## 项目简介

这是一个使用 **C++** 编写的简易 Roguelike 游戏引擎，包含：

- 经典的 **命令行版本**（带彩色显示、FoV 视野、战斗、药水、日志）
- 使用 **raylib** 的 **图形前端**（`gfx_main.cpp`），在窗口中显示同一套游戏逻辑

项目目标是作为一个个人项目，重点体现：

- 游戏引擎 / 游戏逻辑的架构能力
- 基本算法（A\* 寻路、视野 FoV）
- 简单 ECS 思想（实体 + 类型 + 组件风格字段）
- 代码工程化（多文件拆分、Git 管理、README 文档）

---

## 功能特性（当前进度）

### 核心玩法

- 地图为字符网格（`#` 墙、`.` 地板），玩家是 `@`，怪物是 `g` / `o` 等
- 使用 **WASD** 控制玩家移动
- 怪物会根据玩家位置移动，追踪玩家
- 撞到怪物并不会走过去，而是触发**战斗**（攻击并减少怪物 HP）
- 玩家有 **HP / 最大 HP / 攻击力**，怪物也有相应属性
- 玩家死亡后游戏结束

### 地图与视野

- 地图使用 `std::vector<std::string>` 存储
- 维护：
  - `explored[y][x]`：是否曾被看见
  - `visible[y][x]`：当前是否在视野内
- 使用简单的 FoV 算法，只有在玩家视野（可见区域）内的格子才会被正常绘制  
  未探索区域用空白显示，已探索但当前不可见区域用“暗色”显示

### A\* 寻路（pathfinding）

- 在 `pathfinding.cpp` 中实现 A\* 路径搜索：
  - 节点为地图上的可走格子
  - 代价为移动步数
  - 启发函数使用曼哈顿距离
- 可用于：
  - 未来怪物更智能的追踪
  - 玩家自动寻路（如果需要）

### 实体系统（轻量 ECS 风格）

- 定义统一的 `Entity` 结构，包含：

  ```cpp
  enum class EntityType { Player, Monster, Item };

  struct Entity {
      int x, y;
      char glyph;
      bool blocks;
      EntityType type;

      int hp, maxHp, attack;   // 对于 Player / Monster 有意义
      int healAmount;          // 对于 Item（药水）有意义
  };
