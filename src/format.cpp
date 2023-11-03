#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  long hours = seconds / 3600;
  seconds -= 3600 * hours;
  long mins = seconds / 60;
  seconds -= mins * 60;
  std::stringstream out; 
  out << hours << ":";
  out << std::setw(2) << std::setfill('0') << mins; 
  out << ":";
  out << std::setw(2) << std::setfill('0') << seconds;
  return out.str(); 
}