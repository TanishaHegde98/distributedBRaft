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
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<bits/stdc++.h>

//#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpc++/grpc++.h>
//#include <grpcpp/health_check_service_interface.h>
#include "distributeddBRaft.grpc.pb.h"

#include "globals.h"

#define RAFT_LOG_PATH "mylog.log" 

using namespace std;

using distributeddBRaft::AppendEntriesRequest;
using distributeddBRaft::AppendEntriesResponse;
using distributeddBRaft::RequestVoteRequest;
using distributeddBRaft::RequestVoteResponse;
using distributeddBRaft::Entry;
using distributeddBRaft::RaftAPI;
using grpc::Server;
using grpc::ClientContext;
using grpc::Status;
using grpc::ServerContext;


struct LogEntry{
    int64_t term;
    //TODO: rename it to string key ??
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
        //static const string RAFT_LOG_PATH ("mylog.log");
        static const int TIMEOUT = 5000;
        std::vector<std::unique_ptr<RaftAPI::Stub>> stubs;
    public: 
        static atomic<uint64_t> last_comm_time;
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

//defining static variables
atomic<uint64_t> ServerRaft:: last_comm_time(0) ;
atomic<int64_t> ServerRaft:: curTerm(0);
atomic<int64_t> ServerRaft:: voted_for(-1);
atomic<int64_t> ServerRaft:: commit_index(-1);
atomic<int64_t> ServerRaft::last_applied(-1);
int64_t ServerRaft:: current_leader_id = 0;
int64_t ServerRaft::server_id = -1;
int64_t ServerRaft::num_servers = 0;
vector<string> ServerRaft::other_servers;

//defining it in this file
std::vector<struct LogEntry> raft_log;
std::vector<int64_t> nextIndex;
std::vector<int64_t> matchIndex;
std::mutex log_lock;

void ServerRaft::process_server_file(string filename){
      std::ifstream file(filename);
  std::string line;

  ServerRaft::num_servers = 1;
  if (file.is_open()) {
    while(std::getline(file, line)) {
      ServerRaft::num_servers++;
      ServerRaft::other_servers.push_back(line);
    }

    file.close();
  }
}

void ServerRaft:: read_raft_log(){
    int count = 0;
    
    ifstream rf(RAFT_LOG_PATH, ios::out | ios::binary);
    if (!rf) {
        ofstream wf;
        wf.open(RAFT_LOG_PATH, ios::out | ios::binary);
        if(!wf.is_open())
        //create_empty_raft_log();
            return;
        int64_t term=0;
        int64_t vote=-1;
        wf.write((char *) &term, sizeof(int64_t));
        wf.write((char *) &vote, sizeof(int64_t));
        wf.close();
        return;
    }

        rf.read((char *) &curTerm, sizeof(int64_t));
        rf.read((char *) &voted_for, sizeof(int64_t));

        while (!rf.eof())
        {  
            LogEntry l; 
            rf.read((char *) &l, sizeof(LogEntry));
            raft_log.push_back(l);
            count++;
            if (!rf.eof()) 
                break;
        }
        rf.close();
        if(!rf.good()) {
        cout << "Error occurred at reading time!" << endl;
            return;
        }
        printf("Read %d entries from the persistent log!\n", count);
}

void ServerRaft::handleHeartbeats(){

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);
  const int TOTAL_TIMEOUT = TIMEOUT + dist(rng);

//create new stubs
    for (auto it = other_servers.begin(); it != other_servers.end(); it++) {
        cout<<"other server: "<<*it<<endl;
        stubs.push_back(RaftAPI::NewStub(grpc::CreateChannel(*it, grpc::InsecureChannelCredentials())));
        std::cout << "Initializing matchIndex and nextIndex" << std::endl;
        matchIndex.push_back(-1);
        nextIndex.push_back(-1);
      }
    //cout<<"stub size in handle_hearbeat: "<<stubs.size()<<endl;
    //sleep(10);
    while(true){
        bool ret = false;
        if(state == LEADER){
            sendHeartbeats();
            cout<<"before leader sleep\n";
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }else{
            //sleep(TOTAL_TIMEOUT/1000);
            uint64_t last_time = last_comm_time.load();
            if ((get_time() > last_time) && ((get_time() - last_comm_time.load()) >= TOTAL_TIMEOUT)){
            //if(true){
                std::cout << "Trying to become the leader!\n";
                break;
                state = CANDIDATE;
                //ret = startElection();
            }
            if (ret) {
                cout << "Became the leader for term " << curTerm << ", id: " << server_id << "! Democracy works!\n";
            } else {
                //cout << "Did not become the leader\n";
                last_comm_time = get_time();
            }
        }
    }
}

void ServerRaft::sendHeartbeats(){
    std::vector<std::thread> threads;
      int64_t term;

      term = curTerm;

      //std::cout << "Sending Append Entries for term: " << term << std::endl;

      // Spawn threads to do the heartbeat
      //cout<<"stub size: "<<stubs.size();
      for (int i = 0; i < stubs.size(); i++) {
        // cout<<"sendHearB stub ref:"<<stubs[i]<<endl;
        threads.push_back(std::thread(ServerRaft::AppendEntriesRPC,
          std::ref(stubs[i]), i, term));
        //cout<<"in thread spawn for heartbeat "<<term<<endl;
      }
      std::cout << "Append Entries sent for term: " << term << ", waiting for responses" << std::endl;

      for (auto it = threads.begin(); it != threads.end(); it++) {
        (*it).join();
      }

      std::cout << "Append Entries sent for term: " << term << ", received responses" << std::endl;

      return;
}


void ServerRaft::AppendEntriesRPC(std::unique_ptr<RaftAPI::Stub> &stub, int64_t serverIdx, int64_t term){
    AppendEntriesRequest request;
    AppendEntriesResponse response;
    Entry* entry;

    //cout<<"appendEntries stub ref:"<<stub<<endl;

    std::cout << "Sending append entry to " << serverIdx << "for term " << term << std::endl;

    bool success = false;
    int64_t update_index = 0;

    request.set_curr_term(curTerm);
    request.set_leader_id(server_id);
    request.set_leader_commit(commit_index);

      while (!success) {
        // Create ClientContext as unique_ptr here because reusing a context
        // when retrying an RPC can cause gRPC to crash
        auto context = std::make_unique<ClientContext>();

        //log_lock.lock();
        // This represents the last index we are sending to the follower
        update_index = raft_log.size() - 1;

        if (nextIndex[serverIdx] - 1 < 0) {
          request.set_follower_log_term(-1);
          request.set_follower_log_idx(-1);
        } else {
          request.set_follower_log_term(raft_log[nextIndex[serverIdx] - 1].term);
          request.set_follower_log_idx(nextIndex[serverIdx] - 1);
        }

        for (int i = nextIndex[serverIdx]; i < raft_log.size(); i++) {
          entry = request.add_entries();
          entry->set_term(raft_log[i].term);
          entry->set_address(raft_log[i].address);
          entry->set_data(raft_log[i].data); // should send the string hopefully
        }

       // log_lock.unlock();

        stub->AppendEntriesRPC(context.get(), request, &response);
        success = response.success();
        cout << "succcess : " << success <<endl;
        if (!success) {
          std::cout << "Unsuccessful response received from server: " << serverIdx << " for term: " << term << std::endl;

          // If the write fails when we are trying to write the first index,
          // it is likely that the server is down, so stop trying for now
          if (nextIndex[serverIdx] > 0) {
            nextIndex[serverIdx]--;
          } else {
            cout<<"in else of success false, nextIndex[serverIdx]: "<<nextIndex[serverIdx];
            return;
          }
        }
        cout<<"\nSuccessful response "<<serverIdx<<endl;
      }

      nextIndex[serverIdx] = update_index + 1;
      matchIndex[serverIdx] = update_index;
      return;
}

uint64_t ServerRaft::get_time(){
  
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}



//----------------------------------RaftInterfaceImp----------------------------------

class RaftInterfaceImpl final : public RaftAPI::Service {
//   Status RequestVoteRPC(ServerContext *context, const RequestVoteRequest *request,
//                 RequestVoteResponse *reply) override {
//     int64_t requestTerm = request->term();
//     int64_t candidateId = request->candidate_id();
//     int64_t requestLastLogIndex = request->last_log_idx();
//     int64_t requestLastLogTerm = request->last_log_term();
//     int64_t ourLastLogIndex;
//     int64_t ourLastLogTerm;

//     std::cout << "Received RequestVote from " << candidateId << " for term " << requestTerm << std::endl;

//     log_lock.lock();
//     ourLastLogIndex = raft_log.size() - 1;
//     if (ourLastLogIndex >= 0)
//         ourLastLogTerm = raft_log[ourLastLogIndex].term;
//     else
//         ourLastLogTerm = 0;
//     log_lock.unlock();

//     // Use a lock to make sure we don't respond to two simultaneous vote requests
//     vote_lock.lock();

//     if (requestTerm > curTerm) {
//         state = FOLLOWER;
//         // TODO: this method is present in server
//         // update_term_and_voted_for(requestTerm, HAVENT_VOTED);
//     } else if (requestTerm < curTerm) {
//         reply->set_term(curTerm);
//         reply->set_voted(false);
//         goto out;
//     }

//     reply->set_term(curTerm);

//     if (requestLastLogTerm < ourLastLogTerm) {
//         reply->set_voted(false);
//     } else if (requestLastLogTerm == ourLastLogTerm && requestLastLogIndex < ourLastLogIndex) {
//         reply->set_voted(false);
//     } else if (voted_for != -1) {
//         reply->set_voted(false);
//     } else {
//         // update_term_and_voted_for(curTerm, candidateId);
//         reply->set_voted(true);

//         last_comm_time = gettime();
//     }

// out:
//     vote_lock.unlock();
//     return Status::OK;
//   }

  Status AppendEntriesRPC(ServerContext *context, const AppendEntriesRequest * request,
                AppendEntriesResponse *reply) override {

      int64_t requestTerm = request->curr_term();
      int64_t leaderId = request->leader_id();
      int64_t prevLogIndex = request->follower_log_idx();
      int64_t prevLogTerm = request->follower_log_term();

      ServerRaft respRaft;

      std::cout << "Recieved Append Entries from " <<  leaderId << " for term " << requestTerm <<
        ", prevLogIndex: " << prevLogIndex << " and prevLogTerm: " << prevLogTerm << std::endl;

      respRaft.last_comm_time = respRaft.get_time();

      if (requestTerm > respRaft.curTerm) {
        // vote_lock.lock();
        // update_term_and_voted_for(requestTerm, HAVENT_VOTED);
        respRaft.state = FOLLOWER;
        respRaft.current_leader_id = leaderId;
        // vote_lock.unlock();
        //reply->set_success(false);
        //reply->set_term(curTerm);
        cout << "\n1";
      }
  // check on valid term
      if(requestTerm < respRaft.curTerm) {
        reply->set_term(respRaft.curTerm);
        reply->set_success(false);
        cout<<"req term "<<requestTerm<<" less than cur term "<<respRaft.curTerm<< "\n";
        return Status::OK;
      } else {
        
        // vote_lock.lock();
        // curTerm = requestTerm;
        // voted_for = HAVENT_VOTED;
        // state = FOLLOWER;
        // current_leader_id = leaderId;
        // vote_lock.unlock();

        respRaft.state = FOLLOWER;
         cout << "\n2";
        reply->set_term(respRaft.curTerm);
        reply->set_success(true);
      }

      // log_lock.lock();
      // if prevlogindex is more than our last index, or term on prev log index is not same
      if(prevLogIndex >= (int64_t)raft_log.size() || (prevLogIndex >= 0 && raft_log[prevLogIndex].term != prevLogTerm)) {
        reply->set_success(false);
        cout<<"set false for if condition\n";
	      // log_lock.unlock();
        return Status::OK;
      }

      // deleting entries after index with same term

      //TODO: binary write logic
      // if(prevLogIndex+1 < raft_log.size()) {
      //   truncate(RAFT_LOG_FILE, LOG_INTRO_SIZE + ((prevLogIndex + 1) * ENTRY_SIZE));
      //   raft_log.erase(raft_log.begin()+prevLogIndex+1, raft_log.end());
      // }

      // if(request->entries().size() > 0) {
      //   int64_t entryTerm = request->entries(0).term();
      //   if (raft_log[prevLogIndex+1].term != entryTerm) {
      //     raft_log.erase(raft_log.begin()+prevLogIndex+1, raft_log.end());
      //   }
      // }

      // struct LogEntry newEntry;
      // //run a loop, keep on appending entries from WriteRequest
      // for (int i = 0; i < request->entries().size(); i++) { //confirm syntax???
      //   newEntry.term = request->entries(i).term();
      //   newEntry.address = request->entries(i).address();
      //   memcpy(newEntry.data, request->entries(i).data().c_str(), BLOCK_SIZE);
      //   persist_entry_to_log(newEntry);
      //   raft_log.push_back(newEntry);
      //   //commit_index = raft_log.size() - 1;
      // }
      // log_lock.unlock();

      // int64_t leaderCommitIdx = request->leader_commit();
      // if (leaderCommitIdx > commit_index) { //comparison should be with commit index
      //   int64_t new_commit_index = std::min(leaderCommitIdx, (int64_t)(raft_log.size()-1));

      //   // Apply the log entries
      //   apply_entries(commit_index + 1, new_commit_index);

      //   commit_index = new_commit_index;
      // }


    reply->set_success(true);
    return Status::OK;
  }
};
