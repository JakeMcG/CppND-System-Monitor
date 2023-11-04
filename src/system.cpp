#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { 
  vector<int> pids = LinuxParser::Pids();
  for (auto p : pids) {
    // check if the process exists already
    // process objects are persistent to track CPU usage
    if (std::find(pids_.begin(), pids_.end(), p) == pids_.end()) {
      // new pid
      processes_.push_back(Process{p});
      pids_.push_back(p);
    } 
  }
  for (auto &p : processes_) {
    p.UpdateUtilization(); // before sorting
  }
  std::sort(processes_.rbegin(), processes_.rend()); // descending order
  return processes_; 
}

std::string System::Kernel() { 
  return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() { 
  return LinuxParser::TotalProcesses(); 
}

long int System::UpTime() { 
  return LinuxParser::UpTime(); 
}
