#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  void UpdateUtilization();
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
  int pid_;
  int uid_;
  std::string command_;
  std::string user_;
  long prevUpTime;
  long prevActiveJiffies;
  float cpuUtilization_;
};

#endif