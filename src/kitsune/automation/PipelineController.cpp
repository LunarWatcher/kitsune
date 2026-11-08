#include "PipelineController.hpp"
#include "kitsune/api/LuaApi.hpp"

namespace kitsune {

void PipelineController::loadPipelines(LuaApi* api) {
    this->api = api;

    for (auto& lookupDir : conf.pipelineLookupLocations) {
        if (!std::filesystem::is_directory(lookupDir)) {
            minilog::debug("{} is not a folder or does not exist", lookupDir);
            continue;
        }

        minilog::debug("Pipelines: loading from {}", lookupDir);
        
        for (const auto& entry : std::filesystem::directory_iterator(lookupDir)) {
            if (entry.is_directory()) {
                continue;
            }
            auto path = entry.path();
            if (path.extension() != ".lua") {
                continue;
            }

            api->run(path);
        }
    }

}

void PipelineController::runPipeline(const std::string& pipeline) {
    auto& graph = this->pipelines.at(pipeline);
    auto& tasks = graph->getTasks();
    std::vector<std::string> runTaskIds;
    for (auto& [_, task] : tasks) {
        if (task->runSpec == Run::Auto) {
            runTaskIds.push_back(task->id);
        }
    }

    if (!runTaskIds.empty()) {
        runners.push_back(
            graph->start(runTaskIds, this->api)
        );
    }
}

void PipelineController::runTask(const std::string& pipeline, const std::string& taskId) {
    auto& graph = this->pipelines.at(pipeline);
    runners.push_back(
        graph->start({taskId}, this->api)
    );
}

}
