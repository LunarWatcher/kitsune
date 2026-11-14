#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "kitsune/automation/graph/TaskGraph.hpp"
#include "kitsune/components/commands/CommandDispatcher.hpp"

namespace kitsune {

class LuaApi;
class PipelineController {
private:
    std::unordered_map<std::string, std::shared_ptr<TaskGraph>> pipelines;
    std::shared_ptr<CommandDispatcher> dispatcher;
    Config& conf;

    std::vector<std::shared_ptr<TaskRunner>> runners;

    void startRunner(std::shared_ptr<TaskGraph>& graph, const std::vector<std::string>& tasks);
    void initCommands();

    std::optional<Error> dispatchRun(const std::vector<std::string>& args);

    LuaApi* api;
public:
    PipelineController(Config& conf, const std::shared_ptr<CommandDispatcher>& dispatcher);

    void loadPipelines(LuaApi* api);
    void runPipeline(const std::string& pipelineKey);
    void runTask(const std::string& pipeline, const std::string& taskId);

    std::unordered_map<std::string, std::shared_ptr<TaskGraph>>& getPipelines() { return pipelines; }
};

}
