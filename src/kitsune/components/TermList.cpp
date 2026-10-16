#include "TermList.hpp"
#include "giomm/liststore.h"
#include "glib.h"
#include <gtkmm.h>
#include "gtk/gtk.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/expander.h"
#include "gtkmm/signallistitemfactory.h"
#include "gtkmm/singleselection.h"
#include "gtkmm/treeexpander.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "minilog/minilog.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <atomic>
#include <cassert>

namespace kitsune {

TermList::TermList(
    const std::shared_ptr<ModeController>& modeController
)
    : modeController(modeController),
      rootContainer(Gtk::Orientation::HORIZONTAL),
      termContainer(),
      listInputProcessor(
          std::make_shared<ModalInputProcessor>(
              modeController,
              ModalInputType::ForwardNavigation
          )
      )
{
    selectionModel = Gtk::SingleSelection::create();
    // selectionModel->set_can_unselect(false);

    rootDataModel = Gio::ListStore<PipelineModel>::create();
    dataModel = Gtk::TreeListModel::create(
        rootDataModel,
        [](const Glib::RefPtr<Glib::ObjectBase>& it) -> Glib::RefPtr<Gio::ListModel> {
            auto pipeline = std::dynamic_pointer_cast<PipelineModel>(it);
            if (!pipeline) {
                // We don't do nested trees
                return nullptr;
            }
            return pipeline->model;
        },
        false,
        true
    );
    selectionModel->set_model(dataModel);

    this->rootView.set_model(selectionModel);
    this->rootView.add_controller(
        listInputProcessor
    );
    this->rootView.set_single_click_activate();
    this->rootView.set_tab_behavior(Gtk::ListTabBehavior::ITEM);


    this->rootView.signal_activate().connect([this](guint pos) {
        auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(selectionModel->get_object(pos));
        if (auto term = std::dynamic_pointer_cast<TerminalModel>(row->get_item()); term != nullptr) {
            this->termContainer.set_visible_child(term->page->get_name());
            this->active = term;
        } else if (auto pipeline = std::dynamic_pointer_cast<PipelineModel>(row->get_item()); pipeline != nullptr) {
            minilog::error("Clicking the pipeline row has not been implemented yet");
        } else {
            throw std::runtime_error("Received unknown type to signal_activate!");
        }
    });

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem>& ptr) {
        minilog::info("Setup called");
        auto expander = Gtk::make_managed<Gtk::TreeExpander>();
        auto label = Gtk::make_managed<Gtk::Label>();
        expander->set_child(*label);
        ptr->set_child(*expander);
    });
    factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& listItem) {
        minilog::info("Bind called");
        auto ptr = std::dynamic_pointer_cast<Gtk::TreeListRow>(listItem->get_item());
        if (!ptr) {
            return;
        }

        auto commonItem = std::dynamic_pointer_cast<PipelineModelCommon>(ptr->get_item());
        if (commonItem == nullptr) {
            throw std::runtime_error("TermList received row without PipelineModelCommon!");
        }

        auto* expander = dynamic_cast<Gtk::TreeExpander*>(listItem->get_child());
        // seems to be loadbearing for list association. I don't understand why, and the docs are too fucking atrocious
        if (!expander) {
            minilog::error("Got expander == null!");
            return;
        }

        expander->set_list_row(ptr);
        expander->add_controller(
            commonItem->rowProc
        );

        auto* label = (Gtk::Label*) expander->get_child();
        label->set_wrap(true);
        label->set_css_classes({ "pad-medium" });

        if (auto termPtr = std::dynamic_pointer_cast<TerminalModel>(ptr->get_item()); termPtr != nullptr) {
            minilog::info("Setting up for TerminalModel");
            label->set_text(termPtr->termName);
        } else if (auto pipelinePtr = std::dynamic_pointer_cast<PipelineModel>(ptr->get_item()); pipelinePtr != nullptr) {
            minilog::info("Setting up for PipelineModel: {}", ptr->is_expandable());
            label->set_text(pipelinePtr->displayName);
            listItem->set_activatable(true);
            // TODO: input controller should probably be managed by something else maybe? Can we Gtk::make_managed a
            // controller?
        } else {
            minilog::error("Factory received unknown type");
        }
    });
    factory->signal_unbind().connect([this](const Glib::RefPtr<Gtk::ListItem>& listItem) {
        auto ptr = std::dynamic_pointer_cast<Gtk::TreeListRow>(listItem->get_item());
        if (!ptr) {
            return;
        }
        auto* expander = dynamic_cast<Gtk::TreeExpander*>(listItem->get_child());
        expander->remove_controller(
            ((PipelineModelCommon*) ptr->get_item().get())->rowProc
        );
    });
    this->rootView.set_factory(factory);
    this->rootView.set_hexpand(false);
    this->rootView.set_vexpand(true);
    
    this->termContainer.set_expand(true);

    // Force the terminal to be bigger by default. Not sure how much the values matter
    this->termContainer.set_size_request(400, -1);
    this->listOverflow.set_size_request(100, -1);

    this->rootContainer.set_wide_handle();

    this->rootContainer.set_shrink_start_child(false);
    this->rootContainer.set_shrink_end_child(false);
    this->rootContainer.set_resize_start_child(false);
    this->rootContainer.set_resize_end_child(false);

    this->listOverflow.set_child(rootView);
    this->rootContainer.set_start_child(listOverflow);
    this->rootContainer.set_end_child(termContainer);
}

void TermList::addTerminal(const Config& conf) {
    static std::atomic<size_t> i = 0;
    minilog::debug("Adding new terminal");
    // To my great annoyance, under GTK4, Glib::RefPtr is literally just an std::shared_ptr
    auto ptr = Glib::make_refptr_for_instance<TerminalModel>(
        new TerminalModel(
            "Terminal with a really fucking long name",
            conf,
            modeController
        )
    );
    ptr->terminalView->spawn({ "/usr/bin/zsh" });
    ptr->page = this->termContainer.add(*ptr->terminalView->ptr(), std::to_string(++i));
    auto pipeline = std::make_shared<PipelineModel>(modeController);
    pipeline->displayName = std::format("Manual terminal #{}", i.load());
    this->rootDataModel->append(pipeline);
    pipeline->model->append(ptr);
}

Glib::RefPtr<TerminalModel> TermList::addBlankTerminal(
    const Config& conf,
    const std::string& name
) {
    // TODO: this conflicts with addTerminal and means addTerminal cannot be called. Should probably be replaced
    // outright in this function in particular in favour of using <pipeline>-<task ID> as the key
    static std::atomic<size_t> i = 0;
    minilog::debug("Adding new terminal ({})", name);
    // To my great annoyance, under GTK4, Glib::RefPtr is literally just an std::shared_ptr
    auto ptr = Glib::make_refptr_for_instance<TerminalModel>(
        new TerminalModel(
            name,
            conf,
            modeController
        )
    );
    ptr->page = this->termContainer.add(*ptr->terminalView->ptr(), std::to_string(++i));
    // TODO: Look up existing pipelines
    auto pipeline = std::make_shared<PipelineModel>(this->modeController);
    pipeline->model = Gio::ListStore<TerminalModel>::create();
    pipeline->displayName = std::format("Pipeline for task {}", name);
    activePipelines[pipeline->displayName] = pipeline;
    this->rootDataModel->append(pipeline);
    pipeline->model->append(ptr);
    pipeline->items.push_back(ptr);
    return ptr;
}

}
