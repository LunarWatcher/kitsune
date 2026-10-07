#pragma once

#include "kitsune/model/TerminalModel.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace kitsune {


struct Task {
    std::string id;
    std::string displayName;

    std::vector<std::string> depends;

    Glib::RefPtr<TerminalModel> commandExecutionState;
};

// TODO: Rename to pipeline
class TaskGraph {
private:
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;
    std::string name;
public:
    TaskGraph(const std::string& name) : name(name) {}

    std::expected<bool, std::string> push(const std::shared_ptr<Task>& task);
};

}
