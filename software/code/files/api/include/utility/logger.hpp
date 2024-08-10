#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

namespace atum {
class Logger {
  public:
  class LoggerOutput {
    public:
    virtual void log(const std::string &msg) = 0;
  };
  
  enum class LoggerLevel { Debug = 4, Info = 3, Warn = 2, Error = 1, Off = 0 };
  
  Logger(const LoggerLevel iLevel,
         const std::string &iFilename = "",
         std::initializer_list<LoggerOutput *> iOutputs = {});
         
  void debug(const std::string &msg);
  
  void info(const std::string &msg);
  
  void warn(const std::string &msg);
  
  void error(const std::string &msg);
  
  static std::unique_ptr<Logger> makeLog(const LoggerLevel level,
                                         const std::string &filename = "");

  private:
  void log(const std::string &prefix,
           const std::string &msg,
           LoggerLevel msgLevel);

  const LoggerLevel level;
  const std::string filename;
  const std::vector<LoggerOutput *> outputs;
};

} // namespace atum