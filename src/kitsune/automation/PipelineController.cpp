#include "PipelineController.hpp"
#include "kitsune/api/LuaApi.hpp"
#include "kitsune/automation/TaskRunner.hpp"

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
        auto runner = graph->start(runTaskIds, this->api);
        runner->signal_run_complete().connect(
            [this, pipeline](bool success) {
                minilog::info(
                    "Pipeline with ID {} exited with {}",
                    pipeline,
                    success ? "success" : "errors"
                );

                std::erase_if(
                    this->runners,
                    [](const auto& p) {
                        return !p->isRunning();
                    }
                );
                // TODO (clarification): not a fan of this wording, but it was better than "are still running", which is
                // more more obviously false. If a pipeline contains long-running tasks, they are guaranteed to still be
                // running after the initial setup.
                minilog::info(
                    "{} pipelines are still being set up",
                    this->runners.size()
                );
            }
        );
        runner->start();
        runners.push_back(
            runner
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
