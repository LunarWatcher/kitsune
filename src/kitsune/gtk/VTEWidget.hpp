#pragma once

#include "gtkmm/widget.h"
#include "vte/vte.h"
#include <expected>

namespace kitsune {

// TODO: can we `: public Gtk::Widget` instead?
class VTEWidget {
private:
    GtkWidget* terminal;
    Gtk::Widget* widget;

    VTEWidget(GtkWidget* term) noexcept;

    void applyTheming();
public:
    static std::expected<std::shared_ptr<VTEWidget>, std::string> create();
    ~VTEWidget();

    void spawn(const std::vector<std::string>& command);

    std::string getContent();
    Gtk::Widget* ptr() {
        return widget;
    }

};

}
