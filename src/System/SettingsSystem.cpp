#include "SettingsSystem.h"

#include <algorithm>

#include "Core/Resource.h"
#include "Component/Position.h"
#include "Component/Panel.h"
#include "Component/Setting.h"
#include "Component/PendingSetting.h"
#include "Component/BestRecord.h"
#include "Component/TextLabel.h"
#include "Component/InputResponse.h"
#include "Component/Tags.h"
#include "Event/ButtonClickedEvent.h"
#include "Event/BestRecordChangedEvent.h"
#include "Event/SettingsButtonEvent.h"
#include "raylib.h"

namespace {
    const std::vector<std::pair<int,int>> RESOLUTIONS = {
        {1280, 720}, {1600, 900}, {1920, 1080}
    };
}

std::string_view SettingsSystem::name() const { return "settings"; }
Phase SettingsSystem::phase() const { return Phase::Logic; }

// ---------- 辅助 ----------

entt::entity SettingsSystem::findPanel() const {
    auto& reg = *_reg;
    auto view = reg.view<const Panel>();
    for (auto [entity, panel] : view.each()) {
        if (panel.type == PanelType::Settings) return entity;
    }
    return entt::null;
}

void SettingsSystem::populatePending(entt::registry& reg) {
    auto panel = findPanel();
    if (panel == entt::null) return;

    auto settingsView = reg.view<const Settings>();
    if (settingsView.begin() == settingsView.end()) return;
    auto& settings = reg.get<const Settings>(*settingsView.begin());

    int idx = 0;
    for (size_t i = 0; i < RESOLUTIONS.size(); i++) {
        if (RESOLUTIONS[i].first == settings.resolutionW) { idx = (int)i; break; }
    }

    if (reg.all_of<PendingSetting>(panel)) {
        reg.get<PendingSetting>(panel).pendingVolume = settings.volume;
        reg.get<PendingSetting>(panel).pendingResolutionIndex = idx;
    } else {
        reg.emplace<PendingSetting>(panel, settings.volume, idx);
    }
}

void SettingsSystem::updateDisplay(entt::registry& reg) {
    auto panel = findPanel();
    printf("[Settings] display updated\n");
    if (panel == entt::null || !reg.all_of<PendingSetting>(panel)) return;
    auto& pend = reg.get<PendingSetting>(panel);

    auto volView = reg.view<TextLabel, VolumeDisplayTag>();
    for (auto [entity, label] : volView.each()) {
        label.text = std::to_string((int)(pend.pendingVolume * 100)) + "%";
    }

    auto resView = reg.view<TextLabel, ResolutionDisplayTag>();
    for (auto [entity, label] : resView.each()) {
        auto& r = RESOLUTIONS[pend.pendingResolutionIndex];
        label.text = std::to_string(r.first) + "x" + std::to_string(r.second);
    }
}

// ---------- 事件处理 ----------
// 弃用。MSVC连接不稳定。
// void SettingsSystem::onButtonClicked(ButtonClickedEvent& event) {

//     auto& reg = *_reg;
//     auto& res = *_res;


//     // 设置面板按钮被点 → 面板刚打开时填充 PendingSettings
//     // 这段貌似没能起作用
//     if (reg.all_of<SettingsPanelToggleTag>(event.button)) {
//         auto panel = findPanel();
//         if (panel == entt::null) return;
//         auto& p = reg.get<Panel>(panel);
//         if (p.isOpen) populatePending(reg);
//         return;
//     }

//     printf("[Settings] button clicked, entity=%u\n", (unsigned)event.button);

//     auto panel = findPanel();
//     if (panel == entt::null || !reg.all_of<PendingSetting>(panel)) return;
//     auto& pend = reg.get<PendingSetting>(panel);

//     // 确认
//     if (reg.all_of<SettingsConfirmTag>(event.button)) {
//         auto settingsView = reg.view<Settings>();
//         if (settingsView.begin() != settingsView.end()) {
//             auto& settings = reg.get<Settings>(*settingsView.begin());
//             settings.volume = pend.pendingVolume;
//             settings.resolutionW = RESOLUTIONS[pend.pendingResolutionIndex].first;
//             settings.resolutionH = RESOLUTIONS[pend.pendingResolutionIndex].second;

//             SetMasterVolume(settings.volume);
//             SetWindowSize(settings.resolutionW, settings.resolutionH);
//         }
//         reg.remove<PendingSetting>(panel);
//         return;
//     }

//     printf("[Settings] resolution=%d\n", pend.pendingResolutionIndex);

//     // 取消
//     if (reg.all_of<SettingsCancelTag>(event.button)) {
//         reg.remove<PendingSetting>(panel);
//         return;
//     }

//     // 音量
//     if (reg.all_of<VolumeUpTag>(event.button)) {
//         pend.pendingVolume = std::min(1.0f, pend.pendingVolume + 0.1f);
//         updateDisplay(reg);
//         return;
//     }
//     if (reg.all_of<VolumeDownTag>(event.button)) {
//         pend.pendingVolume = std::max(0.0f, pend.pendingVolume - 0.1f);
//         updateDisplay(reg);
//         return;
//     }

//     // 分辨率
//     if (reg.all_of<ResolutionNextTag>(event.button)) {
//         pend.pendingResolutionIndex = std::min((int)RESOLUTIONS.size() - 1,
//                                                 pend.pendingResolutionIndex + 1);
//         updateDisplay(reg);
//         return;
//     }
//     if (reg.all_of<ResolutionPrevTag>(event.button)) {
//         pend.pendingResolutionIndex = std::max(0, pend.pendingResolutionIndex - 1);
//         updateDisplay(reg);
//         return;
//     }

//     // 删除最佳纪录
//     if (reg.all_of<DeleteBestRecordTag>(event.button)) {
//         auto bestView = reg.view<BestRecord>();
//         if (bestView.begin() != bestView.end()) {
//             reg.get<BestRecord>(*bestView.begin()).record.clear();
//             res.events.trigger(BestRecordChangedEvent{});
//         }
//     }
// }

// ---------- 生命周期 ----------

void SettingsSystem::onStart(entt::registry& reg, Resource& res) {
    _reg = &reg;
    _res = &res;
    res.events.sink<ButtonClickedEvent>()
        .connect<&SettingsSystem::onButtonClicked>(*this); 

    _onStartCalled = true;
}

void SettingsSystem::onUpdate(entt::registry& reg, Resource& res) {

    auto panel = findPanel();
    if (panel == entt::null) return;

    auto& p = reg.get<Panel>(panel);

    // 面板刚打开 → 自动填充
    if (p.isOpen && !reg.all_of<PendingSetting>(panel)) {
        populatePending(reg);
    }

    // 面板刚关闭 → 清理
    if (!p.isOpen && reg.all_of<PendingSetting>(panel)) {
        reg.remove<PendingSetting>(panel);
        return;
    }

    // 面板关闭或还没有 PendingSetting → 不处理按钮
    if (!p.isOpen || !reg.all_of<PendingSetting>(panel)) return;
    auto& pend = reg.get<PendingSetting>(panel);

    // --- 按钮点击处理 ---
    // 由于 EnTT signal 的 sink 连接在 MSVC 下行为不可靠，
    // 直接在这里轮询 InputResponse 来响应按钮点击。

    // 确认
    auto confirmView = reg.view<SettingsConfirmTag, const InputResponse>();
    for (auto [entity, input] : confirmView.each()) {
        if (input.clicked) {
            auto settingsView = reg.view<Settings>();
            if (settingsView.begin() != settingsView.end()) {
                auto& settings = reg.get<Settings>(*settingsView.begin());
                settings.volume = pend.pendingVolume;
                settings.resolutionW = RESOLUTIONS[pend.pendingResolutionIndex].first;
                settings.resolutionH = RESOLUTIONS[pend.pendingResolutionIndex].second;

                SetMasterVolume(settings.volume);
                SetWindowSize(settings.resolutionW, settings.resolutionH);
            }
            reg.remove<PendingSetting>(panel);
            return;
        }
    }

    // 取消
    auto cancelView = reg.view<SettingsCancelTag, const InputResponse>();
    for (auto [entity, input] : cancelView.each()) {
        if (input.clicked) {
            reg.remove<PendingSetting>(panel);
            return;
        }
    }

    // 音量+
    auto volUpView = reg.view<VolumeUpTag, const InputResponse>();
    for (auto [entity, input] : volUpView.each()) {
        if (input.clicked) {
            pend.pendingVolume = std::min(1.0f, pend.pendingVolume + 0.1f);
            updateDisplay(reg);
            return;
        }
    }

    // 音量-
    auto volDownView = reg.view<VolumeDownTag, const InputResponse>();
    for (auto [entity, input] : volDownView.each()) {
        if (input.clicked) {
            pend.pendingVolume = std::max(0.0f, pend.pendingVolume - 0.1f);
            updateDisplay(reg);
            return;
        }
    }

    // 分辨率 →
    auto resNextView = reg.view<ResolutionNextTag, const InputResponse>();
    for (auto [entity, input] : resNextView.each()) {
        if (input.clicked) {
            pend.pendingResolutionIndex = std::min((int)RESOLUTIONS.size() - 1,
                                                    pend.pendingResolutionIndex + 1);
            updateDisplay(reg);
            return;
        }
    }

    // 分辨率 ←
    auto resPrevView = reg.view<ResolutionPrevTag, const InputResponse>();
    for (auto [entity, input] : resPrevView.each()) {
        if (input.clicked) {
            pend.pendingResolutionIndex = std::max(0, pend.pendingResolutionIndex - 1);
            updateDisplay(reg);
            return;
        }
    }

    // 删除最佳纪录
    auto delBestView = reg.view<DeleteBestRecordTag, const InputResponse>();
    for (auto [entity, input] : delBestView.each()) {
        if (input.clicked) {
            auto bestView = reg.view<BestRecord>();
            if (bestView.begin() != bestView.end()) {
                reg.get<BestRecord>(*bestView.begin()).record.clear();
                res.events.trigger(BestRecordChangedEvent{});
            }
            return;
        }
    }
}