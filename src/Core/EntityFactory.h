#pragma once 

#include <entt/entt.hpp>
#include "raylib.h"

class Resource;
#include "Component/Panel.h"
#include "Component/Position.h"
#include "Component/TextLabel.h"
#include "Component/Layer.h"


class EntityFactory {

    entt::registry& reg;
    Resource& res;

public:

    EntityFactory(entt::registry& reg, Resource& res);
    ~EntityFactory() = default;

    entt::entity createNeedle(float x, float y, int index);
    entt::entity createDisk(float x, float y, int diskIndex, int totalDisks);
    float diskWidth(int diskIndex, int totalDisks) const;
    entt::entity createSessionState(int diskCount);
    entt::entity createNextSessionConfig(int diskCount);
    entt::entity createBestRecord();
    entt::entity createBackground();

    entt::entity createInstantButton(float x, float y, float w, float h, int layer);
    entt::entity createToggleButton(float x, float y, float w, float h, int layer);
    entt::entity createPanel(float x, float y, float w, float h, int layer, PanelType type);
    entt::entity createOverlay(float x, float y, float w, float h, int layer);
    
    entt::entity createText(float x, float y, const std::string& text, int fontSize, Color color, int layer);
    template<typename... Tags> 
    entt::entity createTextWithTag(float x, float y, const std::string& text, int fontSize, Color color, int layer);
    
    entt::entity createSettings();
    entt::entity createHistoryEntry(int diskCount, int steps, bool completed, const std::string& timestamp);

    void createDisksOnNeedle(entt::entity needle);
    
};


// 模板函数实现
template<typename... Tags>
entt::entity EntityFactory::createTextWithTag(float x, float y, const std::string& text,
                                 int fontSize, Color color, int layer) {
    auto entity = reg.create();
    reg.emplace<Position>(entity, x, y);
    reg.emplace<TextLabel>(entity, text, fontSize, color);
    reg.emplace<Layer>(entity, layer);
    (reg.emplace<Tags>(entity), ...);
    return entity;
}