#define CATCH_CONFIG_RUNNER

#include "catch2/catch_session.hpp"

int main(int argc, const char* argv[]) {
    // TODO: I have looked into integration testing, and it doesn't appear to be possible without a fair bit of
    // work. Though it is fairly trivial to spawn an app, getting it to function like a native app seems to be the big
    // problem. GTK offers some test utils, but the test API is largely just `gtk_test_init()`, which doesn't do all
    // that much really.
    // There were some functions like `gtk_test_find_widget`, but they seem to have been axed after GTK 3, so would need
    // to reimplement a fair bit of functionality in order to be able to make the test suite work (which is very much
    // disappointing).
    // I have failed to find any testing examples under GTK 4, much less that works well with gtkmm, which makes this
    // even harder.
    // Will have to revisit in the future.
    return Catch::Session().run(argc, argv);
}
