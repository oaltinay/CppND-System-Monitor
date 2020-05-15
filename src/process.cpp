#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

#include <iostream>
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() const { return this->pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
  long active_ticks = LinuxParser::ActiveJiffies(pid_);
  long system_ticks = LinuxParser::Jiffies();

  this->util_ = (active_ticks-cached_active_ticks_) / ((float)system_ticks-cached_system_ticks_);
  this->cached_active_ticks_ = active_ticks; 
  this->cached_system_ticks_ = system_ticks;

  return this->util_ ;
};

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram()  { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return  LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process& a)  { 
  return stol(LinuxParser::Ram(pid_)) > stol(LinuxParser::Ram(a.Pid()));
}