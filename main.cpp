#include "application.hpp"
#include <cstdlib>

int main(int argc, char** argv){

    Application app;
    app.init();
    app.run();
    app.quit();

    exit(EXIT_SUCCESS);
}