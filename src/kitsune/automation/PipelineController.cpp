#include "PipelineController.hpp"
#include "kitsune/api/LuaApi.hpp"
#include "kitsune/automation/TaskRunner.hpp"

namespace kitsune {

void PipelineController::startRunner(std::shared_ptr<TaskGraph>& graph, const std::vector<std::string>& tasks)
{
    auto runner = graph->start(tasks, this->api);
    auto pipeline = graph->getId();
    runner->signal_run_complete().connect(
        [this, pipeline](bool success) {
            minilog::info("Pipeline {} exited with {}", pipeline, success ? "success" : "errors");

            std::erase_if(
                this->runners,
                [](const auto& p) {
                    return !p->isRunning();
                });
            // TODO (clarification): not a fan of this wording, but it was better than "are still running", which is
            // more more obviously false. If a pipeline contains long-running tasks, they are guaranteed to still be
            // running after the initial setup.
            minilog::info("{} pipelines are still being set up", this->runners.size());
        });
    runner->start();
    runners.push_back(std::move(runner));
}

std::optional<Error> PipelineController::dispatchRun(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return Error {
            "Expected precisely 2 arguments: pipelineId commandId"
        };
    }

    auto pipeline = this->pipelines.find(args.at(0));
    if (pipeline == this->pipelines.end()) {
        return Error {
            "Failed to find pipeline with ID " + args.at(0)
        };
    }

    if (!pipeline->second->containsTask(args.at(1))) {
        return Error {
            "Failed to find task with ID " + args.at(1)
        };
    }

    startRunner(pipeline->second, { args.at(1) });

    return std::nullopt;
}

void PipelineController::initCommands()
{
    dispatcher->registerCommand(
        "run",
        std::bind(&PipelineController::dispatchRun, this, std::placeholders::_1)
    );
}

PipelineController::PipelineController(Config& conf, const std::shared_ptr<CommandDispatcher>& dispatcher)
    : dispatcher(dispatcher), conf(conf)
{
    initCommands();
}
void PipelineController::loadPipelines(LuaApi* api)
{
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
        startRunner(graph, runTaskIds);
    }
}

void PipelineController::runTask(const std::string& pipeline, const std::string& taskId) {
    auto& graph = this->pipelines.at(pipeline);

    startRunner(graph, { taskId });
}

}
