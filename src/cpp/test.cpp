#include "toolbox.hpp"

int main(int argc, const char *argv[]) {
    using namespace SE7::win32;
    using namespace SE7::interoperability;

    try {
        toolbox t("test.SE7");

        t.create("boing", 27);
    } catch (const std::exception &e) {
        std::cout << e.what() << "\n";
    }

    return 0;
}