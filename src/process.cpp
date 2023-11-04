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

Process::Process(int pid) : pid_(pid), prevUpTime(0), prevActiveJiffies(0), cpuUtilization_(0) {
  uid_ = LinuxParser::Uid(pid_);
  command_ = LinuxParser::Command(pid_);
  user_ = LinuxParser::User(uid_);
}

int Process::Pid() const { 
  return pid_; 
}

float Process::CpuUtilization() const {
  return cpuUtilization_;
}

// update CPU utilization
void Process::UpdateUtilization() { 
  long ut = UpTime();
  long active = LinuxParser::ActiveJiffies(pid_);

  if (ut == 0 || (ut - prevUpTime) == 0) {
    // either inactive or <1 second has passed since last poll
    // update jiffies but not utilization
    prevActiveJiffies = active;
  } else {
    cpuUtilization_ = (active - prevActiveJiffies) / float(sysconf(_SC_CLK_TCK)) / float(ut - prevUpTime);
    prevActiveJiffies = active;
    prevUpTime = ut;
  }
}

string Process::Command() const { 
  return command_;
}

string Process::Ram() const { 
  return LinuxParser::Ram(pid_); 
}

string Process::User() const {
  return user_; 
}

long int Process::UpTime() const { 
  return LinuxParser::UpTime(pid_);
}

bool Process::operator<(Process const& a) const { 
  return this->CpuUtilization() < a.CpuUtilization(); 
}
