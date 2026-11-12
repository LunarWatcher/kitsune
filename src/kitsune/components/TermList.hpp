#pragma once

#include "giomm/liststore.h"
#include "gtkmm/box.h"
#include "gtkmm/button.h"
#include "gtkmm/listitem.h"
#include "gtkmm/listview.h"
#include "gtkmm/paned.h"
#include "gtkmm/treeexpander.h"
#include "gtkmm/singleselection.h"
#include "gtkmm/scrolledwindow.h"
#include <gtkmm/gestureclick.h>
#include "gtkmm/stack.h"
#include "gtkmm/stacksidebar.h"
#include "gtkmm/treelistmodel.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/app/modes/ModeController.hpp"
#include "kitsune/automation/graph/TaskGraph.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <limits>
#include <unordered_map>
#include <memory>

namespace kitsune {

class PipelineController;
class TermList {
private:
    std::unordered_map<std::string, std::shared_ptr<PipelineModel>> activePipelines;
    std::shared_ptr<ModalInputProcessor> listInputProcessor;
    std::shared_ptr<ModeController> modeController;
    std::shared_ptr<PipelineModelCommon> active = nullptr;
    PipelineController* pipelineController;

    Gtk::Box rootContainer;

    Gtk::Paned termSplitView;
    Gtk::ScrolledWindow listOverflow;
    Gtk::ListView rootView;

    Gtk::Stack termContainer;
    
    Glib::RefPtr<Gio::ListStore<PipelineModel>> rootDataModel;
    Glib::RefPtr<Gtk::TreeListModel> dataModel;
    Glib::RefPtr<Gtk::SingleSelection> selectionModel;

    Gtk::Box taskBarContainer;
    Gtk::Button runBtn;

    guint last = std::numeric_limits<guint>::max();

    void initActionBar();
    void initTermView();

    std::shared_ptr<PipelineModel> getOrCreatePipeline(const std::string &key);
    void bindContextMenu(
        Gtk::TreeExpander& item,
        Gtk::ListItem* owningItem
    );

    void run(const std::shared_ptr<PipelineModelCommon>& what);
public:
    TermList(
        const std::shared_ptr<ModeController>& modeController,
        PipelineController* pipelineController
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
