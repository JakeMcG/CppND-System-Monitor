#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  uid_ = LinuxParser::Uid(pid_);
  command_ = LinuxParser::Command(pid_);
}

int Process::Pid() { 
  return pid_; 
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

string Process::Command() { 
  return command_;
}

string Process::Ram() { 
  return LinuxParser::Ram(pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() {
  return uid_; 
}

long int Process::UpTime() { 
  LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }