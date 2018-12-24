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
    emb::host::EmbMessenger messenger(&buffer, [&](std::exception_ptr eptr){
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
    messenger.registerCommand<DelayMs>(4);

   messenger.registerPeriodicCommand<ToggleLed>(250, [](auto&& cmd) {
       std::cout << "Led is " << (cmd->getLedState() ? "On" : "Off") << std::endl;
    });

    std::cout << "Ping...";
    auto pingCommand = std::make_shared<Ping>();
    pingCommand->setCallback<Ping>([](auto&& ping){ std::cout << "Pong" << std::endl; });
    messenger.send(pingCommand);
    pingCommand->wait();

    auto setLedCommand = std::make_shared<SetLed>(true);
    setLedCommand->setCallback<SetLed>([](auto&& setLed){ std::cout << "Led Set to on." << std::endl; });
    messenger.send(setLedCommand);
    setLedCommand->wait();

    auto addCommand = std::make_shared<Add>(0x40, 0x50);
    addCommand->setCallback<Add>([](auto&& add){
        std::cout << add->getA() << " + " << add->getB() << " = " << add->getResult() << std::endl;
    });
    messenger.send(addCommand);
    addCommand->wait();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    messenger.unregisterPeriodicCommand<ToggleLed>();
}
