#include "PipelineController.hpp"
#include "kitsune/api/LuaApi.hpp"

namespace kitsune {

void PipelineController::loadPipelines(LuaApi* api) {
    this->api = api;

    for (auto& lookupDir : conf.pipelineLookupLocations) {
        if (!std::filesystem::is_directory(lookupDir)) {
            minilog::debug("{} is not a folder", lookupDir);
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

    start();
}

void PipelineController::start() {
    // TODO: this should be linked to a button that actually starts a specific pipeline rather than auto meaning run on
    // load. Run on load doesn't scale well if there's a lot of pipelines that may or may not be applicable for a
    // run. This will also require rewriting the ListView to support a tree, so the root task can be linked up with
    // buttons in place of the terminal or something.
    for (auto& [_, graph] : pipelines) {
        auto tasks = graph->getTasks();
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
}

}
