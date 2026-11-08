#pragma once

#include "giomm/liststore.h"
#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "gtkmm/label.h"
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
    Gtk::Label* statusLabel = nullptr;

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
            terminalView->signal_run_status_changed().connect([this](auto) {
                // The underlying value is just an enum that we can access directly, and this function may not actually
                // need to consume it at all.
                // in retrospect, I'm not entirely sure why I made this callback include the enum, but eh, it doesn't
                // matter
                setStatus();
            });
        } else {
            minilog::error(
                "An error happened while spawning the terminal: {}",
                res.error().c_str()
            );
        }
    }

    void setStatus() {
        if (this->statusLabel == nullptr) {
            // Not in view
            return;
        }
        auto state = terminalView->state;

        switch (state) {
        case VTEState::PreLaunch:
            statusLabel->set_text("P");
            statusLabel->set_css_classes({"status-indicator", "pending"});
            break;
        case VTEState::Running:
            statusLabel->set_text("R");
            statusLabel->set_css_classes({"status-indicator", "running"});
            break;
        case VTEState::ExitOk:
            statusLabel->set_text("D");
            statusLabel->set_css_classes({"status-indicator", "done"});
            break;
        case VTEState::ExitError:
            statusLabel->set_text("E");
            statusLabel->set_css_classes({"status-indicator", "failed"});
            break;
        case VTEState::LaunchFail:
            statusLabel->set_text("F");
            statusLabel->set_css_classes({"status-indicator", "failed"});
            break;
        }
    }

    void bind(Gtk::Label* statusLabel) {
        this->statusLabel = statusLabel;

        setStatus();
    }

    void unbind() {
        this->statusLabel = nullptr;
    }
};

struct PipelineModel : public Glib::Object, public PipelineModelCommon {
    std::optional<std::string> pipelineKey;
    std::string displayName;

    std::shared_ptr<Gio::ListStore<TerminalModel>> model = Gio::ListStore<TerminalModel>::create();

    PipelineModel(
        const std::shared_ptr<ModeController>& modeController
    ) : PipelineModelCommon(modeController) {}
};

}
