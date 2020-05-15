#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "format.h"

using std::string;
using std::to_string;
// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
	int hours =(seconds/3600); 
	int mins = (seconds - (3600*hours)) / 60;
	int secs = (seconds - (3600*hours) - (mins*60));

  std::stringstream ss1, ss2, ss3;

  ss1 << std::setw(2) << std::setfill('0') << hours;
  string hours_s = ss1.str();

  ss2 << std::setw(2) << std::setfill('0')<<  mins;
  string mins_s = ss2.str();

  ss3 << std::setw(2) << std::setfill('0')<< secs;
  string secs_s = ss3.str();

  return hours_s + ":" + mins_s + ":" + secs_s; 
}