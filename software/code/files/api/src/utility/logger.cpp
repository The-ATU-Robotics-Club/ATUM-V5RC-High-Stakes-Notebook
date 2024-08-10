#include "logger.hpp"

namespace atum {
Logger::Logger(LoggerLevel iLevel,
               const std::string &iFilename,
               std::initializer_list<Logger::LoggerOutput *> iOutputs) :
    level{iLevel}, filename{iFilename}, outputs{iOutputs} {
  if(filename != "") std::fstream file{filename, std::fstream::out};
}

void Logger::debug(const std::string &msg) {
  log("DEBUG", msg, LoggerLevel::Debug);
}

void Logger::info(const std::string &msg) {
  log("INFO", msg, LoggerLevel::Info);
}

void Logger::warn(const std::string &msg) {
  log("WARN", msg, LoggerLevel::Warn);
}

void Logger::error(const std::string &msg) {
  log("ERROR", msg, LoggerLevel::Error);
}

void Logger::log(const std::string &prefix,
                 const std::string &msg,
                 LoggerLevel msgLevel) {
  if(level < msgLevel) return;
  std::stringstream fmtMsg{};
  fmtMsg << std::setw(5) << prefix << std::setw(0) << ": " + msg << '\n';
  std::cout << fmtMsg.str();
  for(auto output : outputs) output->log(fmtMsg.str());
  if(filename == "") return;
  std::fstream file{filename, std::fstream::app};
  file << fmtMsg.str();
}

std::unique_ptr<Logger> Logger::makeLog(const LoggerLevel level,
                                        const std::string &filename) {
  return std::make_unique<Logger>(level, filename);
}
} // namespace atum