#pragma once

#include "gtkmm/widget.h"
#include "kitsune/config/Config.hpp"
#include <expected>

namespace kitsune {

enum class VTEState {
    PreLaunch,
    Running,
    ExitOk,
    ExitError,
    LaunchFail
};

// TODO: can we `: public Gtk::Widget` instead?
class VTEWidget {
public:
    using SignalRunStatusChanged = sigc::signal<void(VTEState)>;
private:
    SignalRunStatusChanged runStatusChanged;
    GtkWidget* terminal;
    Gtk::Widget* widget;


    VTEWidget(const Config& conf, GtkWidget* term) noexcept;

    void applyTheming(
        const Config& scheme
    );
public:
    VTEState state = VTEState::PreLaunch;

    static std::expected<std::shared_ptr<VTEWidget>, std::string> create(
        const Config& conf
    );
    ~VTEWidget();

    void spawn(const std::vector<std::string>& command);

    std::string getContent();
    Gtk::Widget* ptr() {
        return widget;
    }

    SignalRunStatusChanged signal_run_status_changed() {
        return runStatusChanged;
    }

    bool isRunning() {
        return state == VTEState::Running;
    }

    void setState(VTEState newState);

    void reset() {
        setState(VTEState::PreLaunch);
    }
};

}
