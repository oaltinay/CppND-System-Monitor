#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float totalMemory = 1;
  float freeMemory = 0;
  float buffer = 0;
  string key;
  string value;
  string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          std::stringstream stream(value);
          stream >> totalMemory;
        } 
        else if (key == "MemFree:") {
          std::stringstream stream(value);
          stream >> freeMemory;
        } 
        else if (key == "Buffers:") {
          std::stringstream stream(value);
          stream >> buffer;
        } 
      }
    }
  }
  return 1 - freeMemory / (totalMemory - buffer);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {  
  string line;
  string upTime_s;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime_s;
  } 
  return stol(upTime_s, nullptr, 10);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long total = 0;
  string value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> value) {
      if (i >= 13 && i <= 16) {
        total += stol(value);
      }
      i++;
    }
  }
  return total;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long activeJiffies;
  vector<CPUStates> activeStates = {kUser_,kNice_,kSystem_,kIRQ_,kSoftIRQ_,kSteal_};
  vector<string> utilization = CpuUtilization();
  for (auto state: activeStates) {
    activeJiffies += stol(utilization[state]);
  }
  return activeJiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idleJiffies;
  vector<CPUStates> idleStates = {kIdle_,kIOwait_,};
  vector<string> utilization = CpuUtilization();
  for (auto state: idleStates) {
    idleJiffies += stol(utilization[state]);
  }
  return idleJiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string value;
  vector<string> utilization_data{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        utilization_data.emplace_back(value);
      }
    };
  }
  return utilization_data;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int totalProcesses = 0;
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          std::stringstream stream(value);
          stream >> totalProcesses;
          return totalProcesses;
        }
      }
    }
  }
  return totalProcesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {   
  int runningProcesses = 0;
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line))  {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running"  ) {
          std::stringstream stream(value);
          stream >> runningProcesses;
          return runningProcesses;
        }
      }
    }
  }
  return runningProcesses; 
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) +kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;  
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {   
  int ram = 0;
  std::string line;
  std::string key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ram = std::stoi(value) / 1024;
          return std::to_string(ram);
        }
      }
    }
  }
  return std::to_string(ram);
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {   
  std::string line;
  std::string key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return "";
} 

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {   
  std::string uid = Uid(pid);
  std::string line;
  std::string id, x, users;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> id >> x >> users) {
        if (users == uid) {
          return id;
        }
      }
    }
  }
  return "";
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long int LinuxParser::UpTime(int pid) {
  long int uptime = 0;
  string token;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    for (int i = 0; filestream >> token; ++i) {
      if (i == 13) {
        long int uptime{stol(token)};
        uptime /=sysconf(_SC_CLK_TCK);
        uptime = LinuxParser::UpTime() -uptime;
        return uptime;
      }
    }
  }
  return uptime;
}