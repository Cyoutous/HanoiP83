#pragma once

#include <entt/entt.hpp>

struct SettingsButtonEvent { entt::entity button; };
// EnTT 在 MSVC 下会有严重的执行问题！！！
