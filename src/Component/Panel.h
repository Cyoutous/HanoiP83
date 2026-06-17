#pragma once

enum class PanelType { History, Settings };

struct Panel {
    PanelType type;
    bool isOpen = false;
};