#pragma once

#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "gtkmm/stackpage.h"
#include "kitsune/gtk/VTEWidget.hpp"
namespace kitsune {

struct TerminalModel : public Glib::Object {
    Glib::ustring termName;
    // TODO: consider adding a separate identifier so we can source terminal title changes into this datamodel
    std::shared_ptr<VTEWidget> terminalView;

    Glib::RefPtr<Gtk::StackPage> page;

    TerminalModel(
        const Glib::ustring& termName,
        const Config& config
    ) : termName(termName) {
        auto res = VTEWidget::create(config);

        if (res) {
            terminalView = *res;
        } else {
            g_log(
                nullptr,
                GLogLevelFlags::G_LOG_LEVEL_ERROR,
                "An error happened while spawning the terminal: %s",
                res.error().c_str()
            );
        }
    }
};

}
