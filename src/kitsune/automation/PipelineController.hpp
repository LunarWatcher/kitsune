#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "kitsune/automation/graph/TaskGraph.hpp"

namespace kitsune {

class LuaApi;
class PipelineController {
private:
    std::unordered_map<std::string, std::shared_ptr<TaskGraph>> pipelines;
    Config& conf;

    std::vector<std::shared_ptr<TaskRunner>> runners;

    LuaApi* api;
public:
    PipelineController(
        Config& conf
    ) : conf(conf) {}

    void loadPipelines(LuaApi* api);
    void runPipeline(const std::string& pipelineKey);
    void runTask(const std::string& pipeline, const std::string& taskId);
    
    std::unordered_map<std::string, std::shared_ptr<TaskGraph>>& getPipelines() { return pipelines; }
};

}
