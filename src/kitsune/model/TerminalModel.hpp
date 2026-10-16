#pragma once

#include "giomm/liststore.h"
#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "gtkmm/stackpage.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/gtk/VTEWidget.hpp"
#include "minilog/minilog.hpp"
#include <memory>

namespace kitsune {

struct TaskInfo {
    std::string pipelineKey;
    std::string taskKey;
};

struct PipelineModelCommon {
    Glib::RefPtr<ModalInputProcessor> rowProc;

    PipelineModelCommon(
        const std::shared_ptr<ModeController>& modeController
    ) : rowProc(
        std::make_shared<ModalInputProcessor>(
            modeController,
            ModalInputType::ForwardNavigation
        )
    ) {}
};

struct TerminalModel : public Glib::Object, public PipelineModelCommon {
    Glib::ustring termName;
    // TODO: consider adding a separate identifier so we can source terminal title changes into this datamodel
    std::shared_ptr<VTEWidget> terminalView;

    Glib::RefPtr<Gtk::StackPage> page;
    Glib::RefPtr<ModalInputProcessor> inputProc;

    /**
     * Contains IDs to a corresponding pipeline and task, provided this view was started by a scripted pipeline.
     * For manual terminals, this is undefined.
     */
    std::optional<TaskInfo> taskInfo;

    TerminalModel(
        const Glib::ustring& termName,
        const Config& config,
        const std::shared_ptr<ModeController>& modeController
    ) : PipelineModelCommon(modeController),
        termName(termName),
        inputProc(std::make_shared<ModalInputProcessor>(
                modeController,
                ModalInputType::InsertOnly
            ))

    {
        auto res = VTEWidget::create(config);

        // TODO: we should probably use the create pattern so this can return null and be error handled more properly
        if (res) {
            terminalView = *res;
            terminalView->ptr()->add_controller(inputProc);
        } else {
            minilog::error(
                "An error happened while spawning the terminal: {}",
                res.error().c_str()
            );
        }
    }
};

struct PipelineModel : public Glib::Object, public PipelineModelCommon {
    std::optional<std::string> pipelineKey;
    std::string displayName;

    std::shared_ptr<Gio::ListStore<TerminalModel>> model = Gio::ListStore<TerminalModel>::create();
    std::vector<std::shared_ptr<TerminalModel>> items;

    PipelineModel(
        const std::shared_ptr<ModeController>& modeController
    ) : PipelineModelCommon(modeController) {}
};

}
