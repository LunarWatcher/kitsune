#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace kitsune {


struct Task {
    std::string id;
    std::string displayName;

    std::vector<std::string> depends;
};

class TaskGraph {
private:
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;
public:
};

}
