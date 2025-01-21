#include "adi.hpp"

namespace atum {
ADIExtenderPort::ADIExtenderPort(const std::int8_t smartPort,
                                 const std::uint8_t adiPort,
                                 const Logger::Level loggerLevel) :
    port{smartPort, adiPort}, logger{loggerLevel} {
  pros::Device extender{smartPort};
  if(extender.is_installed()) {
    logger.debug("ADI extender found on port " +
                 std::to_string(extender.get_port()) + ".");
  } else {
    logger.error("ADI extender at port " + std::to_string(extender.get_port()) +
                 " could not be initialized!");
  }
  logger.info("ADI extender contructed with port " +
              std::to_string(extender.get_port()) + ".");
}

ADIExtenderPort::ADIExtenderPort(const std::uint8_t adiPort,
                                 const Logger::Level loggerLevel) :
    logger{loggerLevel} {
  port.second = adiPort;
  const auto extenders{pros::Device::get_all_devices(pros::DeviceType::adi)};
  if(!extenders.size()) {
    logger.error("ADI extender not found!");
    port.first = errorPort;
    return;
  } else if(extenders.size() > 1) {
    logger.warn("Multiple ADI extenders found! Using first port found.");
  }
  port.first = extenders.front().get_port();
  logger.debug("ADI extender found on port " + std::to_string(port.first) +
               ".");
}

pros::adi::ext_adi_port_pair_t ADIExtenderPort::operator()() const {
  return port;
}

} // namespace atum