#include "RecordPopupSystem.h"

#include "Core/Resource.h"

#include "Component/TextLabel.h"
#include "Component/Tags.h"

#include "Event/RecordBrokenEvent.h"
#include "Event/ButtonClickedEvent.h"

std::string_view RecordPopupSystem::name() const { 
    return "recordPopup"; 
}
Phase RecordPopupSystem::phase() const { return Phase::Logic; }

void RecordPopupSystem::onRecordBroken(RecordBrokenEvent& event) {
    auto& reg = *_reg;

    auto view = reg.view<TextLabel, RecordPopupTag>();
    for (auto [entity, label] : view.each()) {
        label.text = (event.type == RecordType::Step)
            ? "New Step \nRecord!" : "New Time \nRecord!";
        label.color = GREEN;
    }
}

void RecordPopupSystem::onResetClicked(ButtonClickedEvent& event) {
    auto& reg = *_reg;
    if (!reg.all_of<ResetButtonTag>(event.button)) return;

    auto view = reg.view<TextLabel, RecordPopupTag>();
    for (auto [entity, label] : view.each()) {
        label.text = "";
    }
}

void RecordPopupSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    res.events.sink<RecordBrokenEvent>()
        .connect<&RecordPopupSystem::onRecordBroken>(*this);
    res.events.sink<ButtonClickedEvent>()
        .connect<&RecordPopupSystem::onResetClicked>(*this);
}

void RecordPopupSystem::onUpdate(entt::registry& reg, Resource& res) {}
