#include<iostream>
#include<vector>
#include<mutex>

extern std::vector<struct LogEntry> raft_log;
extern std::vector<int64_t> nextIndex;
extern std::vector<int64_t> matchIndex;
extern std::mutex log_lock;