#include "TermList.hpp"
#include "giomm/liststore.h"
#include "glib.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/signallistitemfactory.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/log/Logger.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <atomic>

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
    selectionModel->set_can_unselect(false);

    dataModel = Gio::ListStore<TerminalModel>::create();
    selectionModel->set_model(dataModel);

    this->rootView.set_model(selectionModel);
    this->rootView.add_controller(
        listInputProcessor
    );

    const auto factory = Gtk::SignalListItemFactory::create();

    this->rootView.signal_activate().connect([this](guint pos) {
        auto it = std::dynamic_pointer_cast<TerminalModel>(selectionModel->get_object(pos));

        if (it != nullptr) {
            this->termContainer.set_visible_child(it->page->get_name());
        }
    });

    factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem>& ptr) {
        auto label = Gtk::make_managed<Gtk::Label>();
        ptr->set_child(
            *label
        );
    });
    factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& ptr) {
        auto data = std::dynamic_pointer_cast<TerminalModel>(ptr->get_item());

        auto* label = (Gtk::Label*) ptr->get_child();
        label->set_wrap(true);
        label->set_css_classes({ "menu-row" });
        label->set_text(data->termName);
    });
    factory->signal_unbind().connect([this](const Glib::RefPtr<Gtk::ListItem>& ptr) {
        auto data = std::dynamic_pointer_cast<TerminalModel>(ptr->get_item());

        auto* label = (Gtk::Label*) ptr->get_child();
    });

    this->rootView.set_factory(factory);

    this->rootView.set_expand(false);
    this->termContainer.set_expand(true);

    // Force the terminal to be bigger by default. Not sure how much the values matter
    this->termContainer.set_size_request(400, -1);
    this->rootView.set_size_request(100, -1);

    this->rootContainer.set_wide_handle();

    this->rootContainer.set_shrink_start_child(false);
    this->rootContainer.set_shrink_end_child(false);
    this->rootContainer.set_resize_start_child(false);
    this->rootContainer.set_resize_end_child(false);

    this->rootContainer.set_start_child(rootView);
    this->rootContainer.set_end_child(termContainer);
}

void TermList::addTerminal(const Config& conf) {
    static std::atomic<size_t> i = 0;
    logger::debug("Adding new terminal");
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
    this->dataModel->append(
        ptr
    );
}

}
