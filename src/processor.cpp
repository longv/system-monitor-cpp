#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

using std::vector;
using std::string;

// Return the aggregate CPU utilization
float Processor::Utilization() { 
  long totalJiffies = LinuxParser::Jiffies();
  long activeJiffies = LinuxParser::ActiveJiffies();

  float cpuUtil = (float)(activeJiffies - prevActiveJiffies_) / (totalJiffies - prevTotalJiffies_);

  prevActiveJiffies_ = activeJiffies;
  prevTotalJiffies_ = totalJiffies;

  return cpuUtil; 
}
