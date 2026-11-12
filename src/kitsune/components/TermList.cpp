#include "TermList.hpp"
#include "gdk/gdk.h"
#include "giomm/liststore.h"
#include "giomm/simpleactiongroup.h"
#include "glib.h"
#include <gtkmm.h>
#include "gtkmm/enums.h"
#include "gtkmm/gestureclick.h"
#include "gtkmm/label.h"
#include "gtkmm/signallistitemfactory.h"
#include "gtkmm/singleselection.h"
#include "gtkmm/treeexpander.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/automation/PipelineController.hpp"
#include "minilog/minilog.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <atomic>
#include <cassert>

namespace kitsune {


void TermList::bindContextMenu(
    Gtk::TreeExpander& item,
    Gtk::ListItem* owningItem
) {
    std::shared_ptr<Gio::Menu> menuModel = Gio::Menu::create();
    menuModel->append("Run", "popup.run");

    auto menu = Gtk::make_managed<Gtk::PopoverMenu>();
    menu->set_parent(item);
    menu->set_autohide();
    menu->set_has_arrow(false);
    menu->set_menu_model(menuModel);

    auto gestureHandler = Gtk::GestureClick::create();
    gestureHandler->set_button(GDK_BUTTON_SECONDARY);
    gestureHandler->signal_released()
        .connect(
            [menu](int, double x, double y) {
                const Gdk::Rectangle rect(x, y, 1, 1);
                menu->set_pointing_to(rect);
                menu->popup();
            }
        );
    item.add_controller(gestureHandler);

    std::shared_ptr<Gio::SimpleActionGroup> actionGroup = Gio::SimpleActionGroup::create();
    actionGroup->add_action("run", [this, listItem = owningItem]() {
        auto ptr = std::dynamic_pointer_cast<Gtk::TreeListRow>(
            listItem->get_item()
        );
        if (!ptr) {
            return;
        }

        auto commonItem = std::dynamic_pointer_cast<PipelineModelCommon>(ptr->get_item());
        if (commonItem == nullptr) {
            return;
        }

        run(commonItem);
    });
    item.insert_action_group("popup", actionGroup);
}

TermList::TermList(
    const std::shared_ptr<ModeController> &modeController,
    PipelineController* pipelineController
)
    : modeController(modeController),
      rootContainer(Gtk::Orientation::VERTICAL),
      termSplitView(Gtk::Orientation::HORIZONTAL), termContainer(),
      listInputProcessor(std::make_shared<ModalInputProcessor>(
              modeController, ModalInputType::ForwardNavigation)),
      selectionModel(Gtk::SingleSelection::create()),
      taskBarContainer(Gtk::Orientation::HORIZONTAL),
      pipelineController(pipelineController)
{
    initActionBar();
    initTermView();

    this->rootContainer.append(taskBarContainer);
    this->rootContainer.append(termSplitView);

}

void TermList::initActionBar() {
    // TODO: set a different cursor
    // This cannot be done via CSS, and the immediate results I found were C GTK and not gtkmm, and I don't care enough
    // about it to figure it out right now
    runBtn.set_label("Run");
    runBtn.signal_clicked().connect([this]() {
        if (!this->active) {
            // TODO: A toast would be nice, but that's part of libadwaita, which I haven't bothered looking up C++
            // wrappers for.
            minilog::error("No active item");
            return;
        }

        run(active);
    });

    taskBarContainer.set_css_classes({"pad-medium"});
    taskBarContainer.append(runBtn);
}

void TermList::run(const std::shared_ptr<PipelineModelCommon>& what) {
    try {
        if (auto term = std::dynamic_pointer_cast<TerminalModel>(what);
            term != nullptr
           ) {
            if (!term->taskInfo.has_value()) {
                minilog::error("{} is not a managed terminal", term->termName.c_str());
                return;
            }
            pipelineController->runTask(term->taskInfo->pipelineKey, term->taskInfo->taskKey);
        } else if (auto pipeline = std::dynamic_pointer_cast<PipelineModel>(active);
            pipeline != nullptr
                  ) {
            pipelineController->runPipeline(*pipeline->pipelineKey);
        } else {
            throw std::runtime_error("Received unknown type to signal_clicked!");
        }
    } catch (const std::exception& e) {
        minilog::error("Failed to spawn pipeline: {}", e.what());
    }
}

void TermList::initTermView() {
    rootDataModel = Gio::ListStore<PipelineModel>::create();
    dataModel = Gtk::TreeListModel::create(
        rootDataModel,
        [](const Glib::RefPtr<Glib::ObjectBase> &it)
        -> Glib::RefPtr<Gio::ListModel> {
            auto pipeline = std::dynamic_pointer_cast<PipelineModel>(it);
            if (!pipeline) {
                // We don't do nested trees
                return nullptr;
            }
            return pipeline->model;
        },
        false, true);
    selectionModel->set_model(dataModel);

    this->rootView.set_model(selectionModel);
    this->rootView.add_controller(listInputProcessor);
    this->rootView.set_single_click_activate();
    this->rootView.set_tab_behavior(Gtk::ListTabBehavior::ITEM);

    this->rootView.signal_activate().connect(
        [this](guint pos) {
            auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(selectionModel->get_object(pos));
            if (auto term = std::dynamic_pointer_cast<TerminalModel>(row->get_item()); term != nullptr) {
                this->termContainer.set_visible_child(term->page->get_name());
                this->active = term;
            } else if (auto pipeline = std::dynamic_pointer_cast<PipelineModel>(row->get_item()); pipeline != nullptr) {
                this->active = pipeline;
            } else {
                throw std::runtime_error("Received unknown type to signal_activate!");
            }

            if (last < selectionModel->get_n_items()) {
                this->rootView.get_children().at(last)->remove_css_class("active");
            }
            this->rootView.get_children().at(pos)->add_css_class("active");
            last = pos;
        }
    );

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        [this](const Glib::RefPtr<Gtk::ListItem>& listItem) {
            minilog::info("Setup called on {}", (int64_t) listItem.get());
            auto expander = Gtk::make_managed<Gtk::TreeExpander>();
            auto contentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
            auto statusLabel = Gtk::make_managed<Gtk::Label>();
            auto nameLabel = Gtk::make_managed<Gtk::Label>();
            contentBox->append(*statusLabel);
            contentBox->append(*nameLabel);
            expander->set_child(*contentBox);

            // TODO: I tried and failed to use std::weak_ptr here. `expired() == true && lock() == nullptr` in the
            // callback, and I do not understand why. I assume it's something to the effect of the internal pointer
            // storage not actually being a shared_ptr? Which would be stupid, but it's the only thing I can get to
            // vaguely make sense.
            // I _think_ we could use the `listItem` directly here without cycles, since the `make_managed` pointers are
            // just new T() + set_managed, which obviously isn't a shared_ptr, but idk, good practice or something?
            bindContextMenu(
                *expander,
                listItem.get()
            );
            
            listItem->set_child(*expander);
        });
    factory->signal_bind().connect(
        [this](const Glib::RefPtr<Gtk::ListItem>& listItem) {
            minilog::info("Bind called on {}", (int64_t) listItem.get());
            auto listRow = std::dynamic_pointer_cast<Gtk::TreeListRow>(listItem->get_item());
            if (!listRow) {
                return;
            }

            auto commonItem = std::dynamic_pointer_cast<PipelineModelCommon>(listRow->get_item());
            if (commonItem == nullptr) {
                throw std::runtime_error(
                    "TermList received row without PipelineModelCommon!");
            }

            auto* expander = dynamic_cast<Gtk::TreeExpander*>(listItem->get_child());
            // seems to be loadbearing for list association. I don't understand why, and the docs are too fucking atrocious
            if (!expander) {
                minilog::error("Got expander == null!");
                return;
            }

            expander->set_list_row(listRow);
            expander->add_controller(
                commonItem->rowProc
            );

            auto* hl = (Gtk::Box*) expander->get_child();
            auto* statusLabel = (Gtk::Label*) hl->get_children()[0];
            auto* label = (Gtk::Label*) hl->get_children()[1];
            label->set_wrap(true);
            label->set_css_classes({ "pad-medium" });

            if (auto termPtr = std::dynamic_pointer_cast<TerminalModel>(listRow->get_item()); termPtr != nullptr) {
                minilog::info("Setting up for TerminalModel");
                label->set_text(termPtr->termName);

                termPtr->bind(statusLabel);
            } else if (auto pipelinePtr = std::dynamic_pointer_cast<PipelineModel>(listRow->get_item()); pipelinePtr != nullptr) {
                minilog::info("Setting up for PipelineModel: {}",
                    listRow->is_expandable());
                label->set_text(pipelinePtr->displayName);
                // listItem->set_activatable(true);
                // TODO: input controller should probably be managed by something else
                // maybe? Can we Gtk::make_managed a controller?
            } else {
                minilog::error("Factory received unknown type");
            }
        });
    factory->signal_unbind().connect(
        [this](const Glib::RefPtr<Gtk::ListItem> &listItem) {
            auto ptr = std::dynamic_pointer_cast<Gtk::TreeListRow>(listItem->get_item());
            if (!ptr) {
                return;
            }
            auto commonItem = std::dynamic_pointer_cast<PipelineModelCommon>(ptr->get_item());
            if (commonItem == nullptr) {
                throw std::runtime_error(
                    "TermList received row without PipelineModelCommon!");
            }
            auto* expander = dynamic_cast<Gtk::TreeExpander*>(listItem->get_child());
            // seems to be loadbearing for list association. I don't understand why, and the docs are too fucking atrocious
            if (!expander) {
                minilog::error("Got expander == null!");
                return;
            }
            expander->remove_controller(
                commonItem->rowProc
            );

            if (auto termPtr = std::dynamic_pointer_cast<TerminalModel>(ptr->get_item()); termPtr != nullptr) {
                termPtr->unbind();
            }
        });
    this->rootView.set_factory(factory);
    this->rootView.set_hexpand(false);
    this->rootView.set_vexpand(true);

    this->termContainer.set_expand(true);

    // Force the terminal to be bigger by default. Not sure how much the values
    // matter
    this->termContainer.set_size_request(400, -1);
    this->listOverflow.set_size_request(100, -1);

    this->termSplitView.set_wide_handle();

    this->termSplitView.set_shrink_start_child(false);
    this->termSplitView.set_shrink_end_child(false);
    this->termSplitView.set_resize_start_child(false);
    this->termSplitView.set_resize_end_child(false);

    this->listOverflow.set_child(rootView);
    this->termSplitView.set_start_child(listOverflow);
    this->termSplitView.set_end_child(termContainer);
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
    const std::shared_ptr<Task>& task,
    const TaskGraph* graph
) {
    std::string uniqueId = std::format("{}/{}", graph->getName(), task->getId());
    minilog::debug("Adding new terminal ({})", uniqueId);
    // To my great annoyance, under GTK4, Glib::RefPtr is literally just an std::shared_ptr
    auto ptr = Glib::make_refptr_for_instance<TerminalModel>(
        new TerminalModel(
            task->getDisplayName(),
            conf,
            modeController,
            TaskInfo {
                graph->getName(),
                task->getId()
            }
        )
    );
    ptr->page = this->termContainer.add(
        *ptr->terminalView->ptr(),
        uniqueId
    );

    auto pipeline = getOrCreatePipeline(graph->getName());
    pipeline->model->append(ptr);
    return ptr;
}

std::shared_ptr<PipelineModel> TermList::getOrCreatePipeline(const std::string &key) {
    if (auto it = activePipelines.find(key); it != activePipelines.end()) {
        return it->second;
    }

    auto pipeline = std::make_shared<PipelineModel>(this->modeController);
    pipeline->displayName = key;
    pipeline->pipelineKey = key;
    activePipelines[key] = pipeline;
    rootDataModel->append(pipeline);
    return pipeline;
}

}
