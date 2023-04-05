#include<iostream>
#include<vector>
#include<mutex>
#include <memory>
#include <string>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <random>

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpc++/grpc++.h>
//#include <grpcpp/health_check_service_interface.h>
#include "distributeddBRaft.grpc.pb.h"

using namespace std;

using distributeddBRaft::AppendEntriesRequest;
using distributeddBRaft::AppendEntriesResponse;
using distributeddBRaft::RequestVoteRequest;
using distributeddBRaft::RequestVoteResponse;
using distributeddBRaft::Entry;
using distributeddBRaft::RaftAPI;
using grpc::Server;
using grpc::ClientContext;


struct LogEntry{
    int64_t term;
    int64_t address;
    string data;
};

enum server_state {
    LEADER,
    FOLLOWER,
    CANDIDATE
};

// extern std::vector<struct LogEntry> raft_log;
// extern std::vector<int64_t> nextIndex;
// extern std::vector<int64_t> matchIndex;
// extern std::mutex log_lock;
// lock for writing into db?

class ServerRaft{

    private:
        const char * RAFT_LOG_PATH= "mylog.log";
        const int TIMEOUT = 5000;
        std::vector<std::unique_ptr<RaftAPI::Stub>> stubs;
    public: 
        static atomic<int64_t> last_comm_time;
        static atomic<int64_t> curTerm;
        static atomic<int64_t> voted_for;
        static atomic<int64_t> commit_index;
        static atomic<int64_t> last_applied;
        static int64_t current_leader_id;
        static int64_t server_id;
        static int64_t num_servers;
        static vector<string> other_servers;
        enum server_state state;

        //handle_heartbeats;
        void process_server_file(string filename);
        void read_raft_log();
        void handleHeartbeats();
        void sendHeartbeats();
        static uint64_t get_time();
        
    static void AppendEntriesRPC(std::unique_ptr<RaftAPI::Stub> &stub, int64_t serverIdx, int64_t term);

    //all raft function helpers
    // 
};

atomic<int64_t> ServerRaft:: last_comm_time(0) ;
atomic<int64_t> ServerRaft:: curTerm(0);
atomic<int64_t> ServerRaft:: voted_for(-1);
atomic<int64_t> ServerRaft:: commit_index(-1);
atomic<int64_t> ServerRaft::last_applied(-1);
int64_t ServerRaft:: current_leader_id = 0;