#include <iostream>
#include <memory>

#include <EmbMessenger/EmbMessenger.hpp>

#include "HostBuffer.hpp"
#include "Commands.hpp"

int main(int argc, char** argv)
{
    serial_t serial;
    if (serial_open(&serial, "/dev/uno", 9600) < 0) {
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(&serial));
        exit(1);
    }
    
    HostBuffer<64> buffer(&serial);
    emb::EmbMessenger messenger(&buffer, [&](std::exception_ptr eptr){
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
    });

    messenger.registerCommand<Ping>(0);
    messenger.registerCommand<SetLed>(1);
    messenger.registerCommand<ToggleLed>(2);
    messenger.registerCommand<Add>(3);

    using namespace std::chrono_literals;

    auto setLedCommand = std::make_shared<SetLed>(true);
    setLedCommand->setCallback<SetLed>([](auto&& setLed){ std::cout << "Led Set to on." << std::endl; });
    messenger.send(setLedCommand);
    std::this_thread::sleep_for(1s);

    auto toggleLedCommand = std::make_shared<ToggleLed>();
    toggleLedCommand->setCallback<ToggleLed>([](auto&& toggleLed){
        std::cout << "Led State: " << std::boolalpha << toggleLed->getLedState() << std::endl;
    });
    messenger.send(toggleLedCommand);
    std::this_thread::sleep_for(1s);

    auto addCommand = std::make_shared<Add>(0x40, 0x50);
    addCommand->setCallback<Add>([](auto&& add){
        std::cout << "a + b = " << +add->getResult() << std::endl;
    });
    messenger.send(addCommand);
    std::this_thread::sleep_for(1s);
}
