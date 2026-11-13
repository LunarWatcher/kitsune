#pragma once

#include <lua.hpp>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <expected>

#include "Task.hpp"

namespace kitsune {

class LuaApi;
class TaskRunner;

// TODO: Rename to pipeline
class TaskGraph {
private:
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;
    const std::string id;
    const std::string name;
public:
    TaskGraph(
        const std::string& id,
        const std::string& name
    ) : id(id), name(name) {}

    std::expected<bool, std::string> push(
        const std::shared_ptr<Task>& task,
        LuaApi* api
    );

    std::shared_ptr<TaskRunner> start(
        const std::vector<std::string>& taskList,
        LuaApi* api
    );

    const std::string& getId() const { return id; }
    const std::string& getDisplayName() const { return name; }
    const decltype(tasks)& getTasks() const { return tasks; }
};

}
