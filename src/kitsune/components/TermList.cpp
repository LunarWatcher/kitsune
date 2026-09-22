#include "TermList.hpp"
#include "giomm/liststore.h"
#include "glib.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/signallistitemfactory.h"
#include "kitsune/model/TerminalModel.hpp"
#include <iostream>
#include <atomic>

namespace kitsune {

TermList::TermList()
    : rootContainer(Gtk::Orientation::HORIZONTAL),
      termContainer()
{
    selectionModel = Gtk::SingleSelection::create();
    selectionModel->set_can_unselect(false);

    dataModel = Gio::ListStore<TerminalModel>::create();
    selectionModel->set_model(dataModel);
    
    this->rootView.set_model(selectionModel);

    const auto factory = Gtk::SignalListItemFactory::create();

    this->rootView.signal_activate().connect([this](guint pos) {
        auto it = std::dynamic_pointer_cast<TerminalModel>(selectionModel->get_object(pos));

        std::cout << it.get() << std::endl;
        if (it != nullptr) {
            this->termContainer.set_visible_child(it->page->get_name());
        }
    });

    // This is technically a violation of what signal_bind is supposed to do, but it'll fucking suck if I have to use
    // both bind and setup
    factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& ptr) {
        auto data = std::dynamic_pointer_cast<TerminalModel>(ptr->get_item());

        auto label = Gtk::make_managed<Gtk::Label>(data->termName);
        label->set_wrap(true);
        ptr->set_child(
            *label
        );
    });

    this->rootView.set_factory(factory);

    this->termContainer.set_expand(true);

    this->rootContainer.append(rootView);
    this->rootContainer.append(termContainer);
}

void TermList::addTerminal(const Config& conf) {
    static std::atomic<size_t> i = 0;
    g_log(nullptr, GLogLevelFlags::G_LOG_LEVEL_INFO, "Adding terminal");
    // To my great annoyance, under GTK4, Glib::RefPtr is literally just an std::shared_ptr
    auto ptr = Glib::make_refptr_for_instance<TerminalModel>(
        new TerminalModel("Terminal", conf)
    );
    ptr->terminalView->spawn({ "/usr/bin/zsh" });
    ptr->page = this->termContainer.add(*ptr->terminalView->ptr(), std::to_string(++i));
    this->dataModel->append(
        ptr
    );
}

}
