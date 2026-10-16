#pragma once

#include "giomm/liststore.h"
#include "gtkmm/listview.h"
#include "gtkmm/paned.h"
#include "gtkmm/singleselection.h"
#include "gtkmm/scrolledwindow.h"

#include "gtkmm/stack.h"
#include "gtkmm/stacksidebar.h"
#include "gtkmm/treelistmodel.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/app/modes/ModeController.hpp"
#include "kitsune/automation/graph/TaskGraph.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <unordered_map>

namespace kitsune {

class TermList {
private:
    std::shared_ptr<ModeController> modeController;

    Gtk::Paned rootContainer;
    Gtk::ScrolledWindow listOverflow;
    Gtk::ListView rootView;

    Gtk::Stack termContainer;

    std::unordered_map<std::string, std::shared_ptr<PipelineModel>> activePipelines;
    
    Glib::RefPtr<Gio::ListStore<PipelineModel>> rootDataModel;
    Glib::RefPtr<Gtk::TreeListModel> dataModel;
    Glib::RefPtr<Gtk::SingleSelection> selectionModel;

    std::shared_ptr<ModalInputProcessor> listInputProcessor;

    std::shared_ptr<TerminalModel> active = nullptr;

    std::shared_ptr<PipelineModel> getOrCreatePipeline(const std::string& key) {
        if (auto it = activePipelines.find(key); it != activePipelines.end()) {
            return it->second;
        }

        auto pipeline = std::make_shared<PipelineModel>(this->modeController);
        pipeline->displayName = key;
        activePipelines[key] = pipeline;
        rootDataModel->append(pipeline);
        return pipeline;
    }
public:
    TermList(
        const std::shared_ptr<ModeController>& modeController
    );

    Gtk::Widget* root() { return &rootContainer; }
    void addTerminal(const Config& conf);
    Glib::RefPtr<TerminalModel> addBlankTerminal(
        const Config& conf,
        const std::shared_ptr<Task>& task,
        const TaskGraph* graph
    );
};

}
