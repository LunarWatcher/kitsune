#include "KitsuneWindow.hpp"
#include "gtk/gtk.h"
#include "gtkmm/cssprovider.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"

namespace kitsune {

MainWindow::MainWindow(
    const Glib::RefPtr<Gtk::Application>& app,
    const EnvConfig& envConfig
) :
    ApplicationWindow(app),
    modeController(new ModeController()),
    fallbackInputProcessor(
        new ModalInputProcessor(modeController, ModalInputType::Window)
    ),
    terminals(modeController),
    modeline(modeController, this),
    root(Gtk::Orientation::VERTICAL, 0),
    api(
        &terminals, &conf
    )
{
    api.loadConfig(envConfig.configRoot);

    set_title("Kitsune");
    set_default_size(720, 480);

    initRootContainer();
    loadCSS();

    for (size_t i = 0; i < 3; ++i) {
        terminals.addTerminal(conf);
    }

    add_controller(fallbackInputProcessor);

    // Init event-driven stuff
    modeController->setMode(Mode::Normal);

    api.run("../.kitsune/pipelines/dev.lua");
}

void MainWindow::loadCSS() {
    auto css = Gtk::CssProvider::create();
    css->load_from_string(
        #include "AppStyle.css"
    );

    Gtk::StyleContext::add_provider_for_display(
        this->get_display(),
        css,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void MainWindow::initRootContainer() {
    root.append(*terminals.root());
    root.append(*modeline.root());
    set_child(root);
}

}
