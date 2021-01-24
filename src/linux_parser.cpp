#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line, memName, memUnit;
  int memTotal, memFree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestreamTotal(line);
    linestreamTotal >> memName >> memTotal >> memUnit;

    std::getline(filestream, line);
    std::istringstream linestreamFree(line);
    linestreamFree >> memName >> memFree >> memUnit;
  }

  return (float)(memTotal - memFree) / memTotal; 
}

long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return uptime;
}

long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  string line, ignore;
  long utime, stime;

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);
    for (int i = 0; i < 15; i++) {
      if (i == 13) {
        linestream >> utime;
      } else if (i == 14) {
        linestream >> stime;
      } else {
        linestream >> ignore;
      }
    }
  }

  return utime + stime;
}

long LinuxParser::ActiveJiffies() { 
  vector<string> stats = LinuxParser::CpuUtilization();
  int user = std::stoi(stats[0]);
  int nice = std::stoi(stats[1]);
  int system = std::stoi(stats[2]);
  int irq = std::stoi(stats[5]);
  int softIrq = std::stoi(stats[6]);
  int steal = std::stoi(stats[7]);

  return user + nice + system + irq + softIrq + steal;
}

long LinuxParser::IdleJiffies() { 
  vector<string> stats = LinuxParser::CpuUtilization();
  int idle = std::stoi(stats[3]);
  int iowait = std::stoi(stats[4]);

  return idle + iowait; 
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<string> stats;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    std::getline(filestream, line);
    
    std::istringstream linestream(line);
    string stat;
    
    linestream >> stat; // Skip "cpu"
    while (linestream >> stat) {
      stats.push_back(stat);
    }
  }
  
  return stats; 
}

int LinuxParser::TotalProcesses() { 
  string line, processes;
  int numOfProcesses;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("processes") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> processes >> numOfProcesses; 
        break;
      }
    }
  }
  
  return numOfProcesses; 
}

int LinuxParser::RunningProcesses() { 
  string line, processes;
  int numOfRunningProcesses;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("procs_running") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> processes >> numOfRunningProcesses; 
        break;
      }
    }
  }

  return numOfRunningProcesses; 
}

string LinuxParser::Command(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  string line;

  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
    
  return line; 
}

string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string line, ramName;
  int ram;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("VmSize") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> ramName >> ram; 
        break;
      }
    }
  }
  
  return std::to_string((int)(ram / std::pow(10, 3))); 
}

string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string line, uidName, uid;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("Uid") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> uidName >> uid; 
        break;
      }
    }
  }
  
  return uid; 
}

string LinuxParser::User(int pid) { 
  std::ifstream filestream(kPasswordPath);
  string uid = LinuxParser::Uid(pid);
  string line, user;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(uid) != std::string::npos) {
        int colonPos = line.find(":");
        user = line.substr(0, colonPos);
        break;
      }
    }
  }

  return user; 
}

long LinuxParser::UpTime(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  string line, uptime;

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> uptime;
    }
  }

  return std::stol(uptime) / sysconf(_SC_CLK_TCK); 
}