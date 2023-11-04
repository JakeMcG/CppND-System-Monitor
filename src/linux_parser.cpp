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
  value = "0"; // safe default for stoi, stof etc calls
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

string getNthToken(std::string filePath, int n) { 
  std::ifstream file(filePath);
  std::string line, token;
  token = "0"; // safe default for stoul etc
  if (std::getline(file, line)) {
    std::stringstream s(line);
    for (auto i = 0; i < n; i++) {
      s >> token; // nth token
    }
  }
  return token;
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

long LinuxParser::ActiveJiffies(int pid) { 
  std::string statFile = kProcDirectory + std::to_string(pid) + kStatFilename;
  long utime = stoul(getNthToken(statFile, 14)); // ticks in user code
  long stime = stoul(getNthToken(statFile, 15)); // ticks in kernel code
  // exclude child processes
  return utime + stime;
}

std::string LinuxParser::readCpuInfo() {
  std::ifstream file(kProcDirectory + kStatFilename);
  std::string line, info;
  if (file.is_open()) {
    if (std::getline(file, line)) { // cpu info on first line
      std::istringstream s(line);
      info = s.str();
    }
  }
  return info;
}

// credit: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::ActiveJiffies() { 
  auto info = readCpuInfo(); 
  std::istringstream ss(info);
  std::string cpu;
  long tuser, tnice, tsys, tidle, twait, tirq, tsoft, tsteal, tguest, tgn;
  ss >> cpu >> tuser >> tnice >> tsys >> tidle >> twait >> tirq >> tsoft >> tsteal >> tguest >> tgn;
  return tuser + tnice + tsys + tirq + tsoft + tsteal;
}

long LinuxParser::IdleJiffies() { 
  auto info = readCpuInfo(); 
  std::istringstream ss(info);
  std::string cpu;
  long tuser, tnice, tsys, tidle, twait, tirq, tsoft, tsteal, tguest, tgn;
  ss >> cpu >> tuser >> tnice >> tsys >> tidle >> twait >> tirq >> tsoft >> tsteal >> tguest >> tgn;
  return tidle + twait;
}

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

string LinuxParser::Ram(int pid) { 
  string kb = getKeyValue(kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:");
  std::stringstream ss;
  ss << stof(kb)/1024; // format number of sig figs
  return ss.str(); // return megabytes
}

int LinuxParser::Uid(int pid) { 
  return stoi(getKeyValue(kProcDirectory + std::to_string(pid) + kStatusFilename, "Uid:"));
}

string LinuxParser::User(int uid) { 
  std::ifstream file(kPasswordPath);
  std::string line, user;
  std::stringstream query;
  query << ":x:" << uid << ":" << uid; // search query in line
  while (std::getline(file, line)) {
    auto i = line.find(query.str());
    if (i != std::string::npos) { // found user id in line
      user = line.substr(0, i);
      break;
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) { 
  long procStartTime = stoul(getNthToken(kProcDirectory + std::to_string(pid) + kStatFilename, 22));
  long sysUpTime = LinuxParser::UpTime();
  return sysUpTime - procStartTime / sysconf(_SC_CLK_TCK);
}
