#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const { 
  long uptime = LinuxParser::UpTime();
  long processUptime = LinuxParser::UpTime(pid_);
  long processActiveJiffies = LinuxParser::ActiveJiffies(pid_);
  
  long seconds = uptime - processUptime;

  float processCpuUtil = (float) (processActiveJiffies / sysconf(_SC_CLK_TCK)) / seconds;  
  
  return processCpuUtil; 
}

string Process::Command() { 
  return LinuxParser::Command(pid_); 
}

string Process::Ram() { 
  return LinuxParser::Ram(pid_); 
}

string Process::User() { 
  return LinuxParser::User(pid_); 
}

long Process::UpTime() { 
  return LinuxParser::UpTime(pid_); 
}

bool Process::operator<(Process const& a) const { 
  return CpuUtilization() < a.CpuUtilization(); 
}