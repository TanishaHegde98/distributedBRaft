#include<iostream>
#include<vector>
#include<mutex>
#define DB_SUCCESS 1
#define NOT_PRIMARY 2
#define DB_FAIL -2

extern std::vector<struct LogEntry> raft_log;
extern std::vector<int64_t> nextIndex;
extern std::vector<int64_t> matchIndex;
extern std::mutex log_lock;

bool db_put(std::string key, std::string value, int &errorCode, int &returnCode);