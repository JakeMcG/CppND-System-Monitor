#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {
  Utilization(); // upon construction, update the prevTotal and prevActive members
}

// computes real-time utilization
// credit: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 
  auto active = LinuxParser::ActiveJiffies();
  auto idle = LinuxParser::IdleJiffies();
  auto total = active + idle;

  if (total == prevTotal) {
    return 0;
  }

  auto util = (active - prevActive) / float(total - prevTotal);

  prevActive = active;
  prevTotal = total;

  return util;
}