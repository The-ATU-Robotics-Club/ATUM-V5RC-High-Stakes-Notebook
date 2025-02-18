#include "led.hpp"

namespace atum {
LED::LED(const std::uint8_t port, const std::uint32_t length) :
    led{port, length} {
  logger.debug("LED on port " + std::to_string(std::get<1>(led.get_port())) +
               " has been constructed.");
}

LED::LED(const ADIExtenderPort &port, const std::uint32_t length) :
    led{port(), length} {
  logger.debug("LED on port " + std::to_string(std::get<1>(led.get_port())) +
               " has been constructed.");
}

void LED::setColor(const std::uint32_t iColor) {
  color = iColor;
  on();
}

void LED::on() {
  led.set_all(color);
  led.update();
}

void LED::off() {
  led.set_all(0x000000);
}
} // namespace atum