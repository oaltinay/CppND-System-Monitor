#include "processor.h"
#include "linux_parser.h"

#include <iostream>

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 

  long active_ticks = LinuxParser::ActiveJiffies();
  long idle_ticks = LinuxParser::IdleJiffies();

  long duration_active = active_ticks - cached_active_ticks_;
  long duration_idle = idle_ticks - cached_idle_ticks_;

  this->cached_active_ticks_ = active_ticks;
  this->cached_idle_ticks_ = idle_ticks;
  
  return static_cast<float>(duration_active) / (duration_active + duration_idle);
}