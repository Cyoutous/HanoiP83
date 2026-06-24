#pragma once

enum class RecordType { Step, Time };

struct RecordBrokenEvent {
    RecordType type;
};
