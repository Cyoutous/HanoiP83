# HanoiP83

一个基于 C++17、EnTT ECS 和 raylib 的汉诺塔益智游戏。

![HanoiP83](assets/Texture/cover.png)

## 功能

- 经典三柱汉诺塔，支持 3–9 层盘子
- 步数计数器与计时器，按盘子数量分别记录最佳成绩
- 自动演示模式，一键展示最优解法
- 窗口关闭后自动存档，下次启动可继续未完成的游戏
- 最佳纪录追踪（最少步数 & 最短时间）
- 基于精灵图的贴图渲染，支持按层级排序绘制
- 窗口缩放锁定 16:9 比例

## 操作

| 操作 | 按钮 |
|------|------|
| 选中/移动盘子 | 点击柱子 |
| 调整盘子数量 | 底部左/右箭头按钮 |
| 切换自动演示 | 自动演示按钮 |
| 重置游戏 | 重置按钮 |
| 打开帮助面板 | 右上角 ? 按钮 |

## 构建

**环境要求：**
- CMake ≥ 3.15
- Conan ≥ 2.0
- MSVC（Visual Studio 2022/2026）或 GCC（MSYS2 MinGW）

**构建步骤：**

```bash
# 安装依赖
conan install . --output-folder=. --build=missing

# 配置与编译
cmake --preset conan-default
cmake --build build --config Release

# 运行
./build/Release/hanoip83.exe
```

## 项目结构

基于 [EnTT](https://github.com/skypjack/entt) 的 ECS（实体-组件-系统）架构：

```
src/
├── Core/         应用入口、资源管理、实体工厂、存档管理、场景配置
├── System/       输入、按钮、柱交互、移动执行、补间动画、渲染等系统
├── Component/    位置、层级、精灵状态、可点击、标签等纯数据组件
├── Event/        按钮点击、关卡状态变更、最佳纪录变更等事件
├── Type/         移动指令、存档缓存等自定义类型
└── main.cpp
```

三个阶段按顺序执行：

- **Logic**：输入检测、按钮响应、补间动画
- **FixedUpdate**：移动执行、快照、游戏状态检测、计时器
- **Render**：统一的精灵/文字按层级排序绘制

## 许可

- **代码**：[MIT License](LICENSE)
- **美术素材**（`assets/Texture/`）：[CC BY 4.0](assets/Texture/LICENSE)
