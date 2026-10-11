#pragma once

#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "gtkmm/stackpage.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/gtk/VTEWidget.hpp"
#include "minilog/minilog.hpp"
#include <memory>
namespace kitsune {

struct TerminalModel : public Glib::Object {
    Glib::ustring termName;
    // TODO: consider adding a separate identifier so we can source terminal title changes into this datamodel
    std::shared_ptr<VTEWidget> terminalView;

    Glib::RefPtr<Gtk::StackPage> page;
    Glib::RefPtr<ModalInputProcessor> inputProc;
    Glib::RefPtr<ModalInputProcessor> rowProc;

    TerminalModel(
        const Glib::ustring& termName,
        const Config& config,
        const std::shared_ptr<ModeController>& modeController
    ) : termName(termName),
        inputProc(std::make_shared<ModalInputProcessor>(
                modeController,
                ModalInputType::InsertOnly
            )),
        rowProc(std::make_shared<ModalInputProcessor>(
                modeController,
                ModalInputType::ForwardNavigation
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

}
