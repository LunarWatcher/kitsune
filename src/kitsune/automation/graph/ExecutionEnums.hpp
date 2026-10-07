#pragma once

namespace kitsune {

enum class Run {
    Auto,
    OnDemand,
    DependsOnly
};

enum class Notify {
    OnExit = 0,
    OnError = 1,
    Never = 10,
};

}
