#include "SceneSetup.h"
#include "Core/Resource.h"
#include "Core/EntityFactory.h"
#include "Component/Tags.h"
#include "Component/Panel.h"

void SceneSetup::build(entt::registry& reg, Resource& res) {
    auto& f = res.factory;

    // 背景
    //f.createBackground();

    // 三根柱子
    //f.createNeedle(260, 360, 0);   // 左
    // 中
    // 右

    // 底部按钮栏
    // 左箭头
    // 重置
    // 历史面板
    // 设置面板
    // 自动演示（二态）
    // 右箭头
    // 关闭按钮

    // 文字标签：静态


    // 文字标签：带 Tag 动态


    // 面板


    // 遮罩


    // 关卡状态 & 最佳记录


    // 设置

}