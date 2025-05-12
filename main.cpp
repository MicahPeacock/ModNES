#include "cartridge.hpp"

int main(int argc, char* argv[]) {
    auto cartridge = nes::Cartridge::from_file(argv[1]);
    return 0;
}
