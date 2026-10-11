#pragma once

namespace kitsune {

enum class Run {
    Auto = 0,
    OnDemand = 1,
    DependsOnly = 2,
};

enum class Rerun {
    Always = 0,
    IfCompleted = 1,
    Never = 2,
};

enum class Notify {
    OnExit = 0,
    OnError = 1,
    Never = 10,
};

}
