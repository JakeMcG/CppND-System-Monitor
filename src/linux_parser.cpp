#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iomanip>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string getKeyValue(string filePath, string key) {
  std::ifstream file(filePath);
  std::string line, testKey, value;
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream s(line);
      s >> testKey;
      if (testKey == key) {
        s >> value;
        break;
      }
    }
  }
  return value;
}

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

string LinuxParser::Kernel() {
  string os, kernel, version;
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

// output: fractional (0-1)
float LinuxParser::MemoryUtilization() { 
  float memTotal, memFree;
  memTotal = stof(getKeyValue(kProcDirectory + kMeminfoFilename, "MemTotal:"));
  memFree = stof(getKeyValue(kProcDirectory + kMeminfoFilename, "MemFree:"));
  return (memTotal - memFree) / memTotal;
}

long LinuxParser::UpTime() { 
  std::ifstream file(kProcDirectory + kUptimeFilename);
  std::string line;
  long uptime;
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream ss(line);
      if (ss >> uptime) {
        break;
      }
    }
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() { 
  return stoi(getKeyValue(kProcDirectory + kStatFilename, "processes"));
}

int LinuxParser::RunningProcesses() { 
  return stoi(getKeyValue(kProcDirectory + kStatFilename, "procs_running"));
}

string LinuxParser::Command(int pid) { 
  std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string line;
  if (file.is_open()) {
   std::getline(file, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string kb = getKeyValue(kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:");
  std::stringstream ss;
  ss << stof(kb)/1024; // format number of sig figs
  return ss.str(); // return megabytes
}

// TODO: get user NAME
string LinuxParser::Uid(int pid) { 
  return getKeyValue(kProcDirectory + std::to_string(pid) + kStatusFilename, "Uid:");
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

long LinuxParser::UpTime(int pid) { 
  long ut;
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line, token;
  if (std::getline(file, line)) {
    std::stringstream s(line);
    for (auto i = 0; i < 22; i++) {
      s >> token; // 22nd token
    }
    ut = stoi(token) / sysconf(_SC_CLK_TCK);
  }
  return ut;
}
