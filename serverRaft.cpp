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
#define IP_SIZE 22
#define NOTVOTED ""
#define LOG_OFFSET (sizeof(int64_t)+IP_SIZE)
#define ENTRY_SIZE sizeof(LogEntry)

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
    //Problem logEntry should be constant for truncatr
    char address[4096];
    char data[4096];
    // string address = "";
    // string data = "";
};

enum server_state {
    LEADER,
    FOLLOWER,
    CANDIDATE
};

class ServerRaft{

    private:
        //static const string RAFT_LOG_PATH ("mylog.log");
        static const int64_t TIMEOUT = 5000;
        std::vector<std::unique_ptr<RaftAPI::Stub>> stubs;
    public: 
        static atomic<uint64_t> last_comm_time;
        static atomic<int64_t> curTerm;
        static atomic<int64_t> commit_index;
        static atomic<int64_t> last_applied;
        static string current_leader_id;
        static string server_id;
        static string raftLogPath;
        static int64_t num_servers;
        static vector<string> other_servers;
        static char voted_for[IP_SIZE] ;
        enum server_state state;

        //raft helper functions
        void process_server_file(string filename);
        void read_raft_log();
        void handleHeartbeats();
        void sendHeartbeats();
        static uint64_t get_time();
        static void set_serverId(string serverId);
        static void set_raftLogPath();
        void update_term_and_voted_for(int64_t newTerm, char votedFor[]);
        void write_entry_to_log(LogEntry lE);
        void commit_thread();
        
    static void AppendEntriesRPC(std::unique_ptr<RaftAPI::Stub> &stub, int64_t serverIdx, int64_t term);

    //all raft function helpers
    // 
};

//defining static variables
atomic<uint64_t> ServerRaft:: last_comm_time(0) ;
atomic<int64_t> ServerRaft:: curTerm(0);
atomic<int64_t> ServerRaft:: commit_index(-1);
atomic<int64_t> ServerRaft::last_applied(-1);
string ServerRaft:: current_leader_id{""};
string ServerRaft::server_id("");
string ServerRaft::raftLogPath("");
int64_t ServerRaft::num_servers = 0;
vector<string> ServerRaft::other_servers;
char ServerRaft:: voted_for[IP_SIZE]={0};

//defining it in this file
std::vector<struct LogEntry> raft_log;
std::vector<int64_t> nextIndex;
std::vector<int64_t> matchIndex;
std::mutex log_lock;
std::mutex vote_lock;

void ServerRaft::set_serverId(string serverId){
  server_id = serverId;
  set_raftLogPath();
}

void ServerRaft::set_raftLogPath(){
  string mylog("mylog");
  string ext(".log");
  raftLogPath = mylog+server_id+ext;
  // cout<<"log path: "<<raftLogPath<<endl;
}

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


void ServerRaft::commit_thread() {
    int64_t last_log_index = 0;
    int64_t majority = (num_servers / 2) + 1;
    int64_t votes;

    while (true) {
        if (state != LEADER)
            goto sleep;

        last_log_index = raft_log.size() - 1;
        for (int i = last_log_index; i > commit_index; i--) {
            // Start with 1 vote counting ourselves
            votes = 1;
            for (int j = 0; j < matchIndex.size(); j++) {
                if (matchIndex[j] >= i)
                    votes++;
            }

            if (votes >= majority) {
                // We can only commit if the log has an entry of out term
                int64_t term;

                log_lock.lock();
                term = raft_log[i].term;
                log_lock.unlock();

                if (term == curTerm) {
                    // apply_entries(commit_index + 1, i);
                    commit_index = i;
                }
            }
        }
sleep:
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ServerRaft:: read_raft_log(){
    int count = 0;
    // cout<<"int read log\n";
    ifstream rf(raftLogPath.c_str(), ios::out | ios::binary);
    if (!rf) {
        ofstream wf;
        wf.open(raftLogPath.c_str(), ios::out | ios::binary);
        if(!wf.is_open()){
          cout<<"wf null";
          return;
        }
        //create_empty_raft_log();
        int64_t term=0;
        char vote[IP_SIZE] = NOTVOTED;
        wf.write((char *) &term, sizeof(int64_t));
        wf.write((char *) vote, IP_SIZE);
        wf.close();

        // //Read what is written:
        // ifstream of(raftLogPath.c_str(), ios::out | ios::binary);
        // if(!of.is_open())
        //   return;
        // int64_t term1;
        // char vote1[IP_SIZE]={0};
        // of.read((char *) &term1, sizeof(int64_t));
        // of.read((char *) vote1, IP_SIZE);
        // of.close();
        // string vf(vote1);
        // cout<<"Term in file: "<<term1<<"\nVote in file: "<<vf<<endl;
        return;
    }

    rf.read((char *) &curTerm, sizeof(int64_t));
    rf.read((char *) voted_for, sizeof(voted_for));
    string vf(voted_for);
    cout<<"on read Term in file: "<<curTerm<<"\nVote in file: "<<vf<<endl;

    while (!rf.eof())
    {  
        LogEntry l; 
        rf.read((char *) &l, sizeof(LogEntry));
        cout << l.term << l.address << l.data << endl;
        raft_log.push_back(l);
        count++;
        //TODO: reading last line twice

        // if (!rf.eof()) 
        //     break;
    }
    raft_log.pop_back();
    rf.close();
    cout<< "Read "<< raft_log.size() << " entries from the persistent log!\n";
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
            // sleep(1000);
            int64_t last_time = last_comm_time.load();
            int64_t cur_time = get_time();
            int64_t diff = (cur_time - last_time);

            if (cur_time > last_time && cur_time - last_time >= TOTAL_TIMEOUT){
              std::cout << "Trying to become the leader!\n";
              state = CANDIDATE;
              //ret = startElection();
            
              if (ret) {
                  cout << "Became the leader for term " << curTerm << ", id: " << server_id << "! Democracy works!\n";
              } else {
                  cout << "Did not become the leader\n";
                  last_comm_time = get_time();
              }
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
        log_lock.lock();
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
          // string add(raft_log[i].address);
          entry->set_address(raft_log[i].address);
          // string d(raft_log[i].data);
          entry->set_data(raft_log[i].data); // should send the string hopefully
        }
        log_lock.unlock();

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

void ServerRaft::update_term_and_voted_for(int64_t newTerm, char votedFor[]){
  ofstream wf (raftLogPath.c_str(), ios::out | ios::binary);
  if(!wf.is_open())
    return;
  wf.seekp(0,ios::beg);
  wf.write((char *) &newTerm, sizeof(int64_t));
  wf.write((char *) votedFor, IP_SIZE);
  wf.close();

  //Read the updated file
  ifstream rf(raftLogPath.c_str(), ios::out | ios::binary);
  if(!rf.is_open())
    return;
  int64_t term;
  char vote[IP_SIZE];
  rf.read((char *) &term, sizeof(int64_t));
  rf.read((char *) vote, IP_SIZE);
  rf.close();
  string vf(vote);
  cout<<"Update: Term in file: "<<term<<"\nVote in file: "<<vf<<endl;
}

void ServerRaft::write_entry_to_log(LogEntry lE){
  ofstream wf (raftLogPath, ios_base::app | ios::binary);
  if(wf.is_open()) {
      wf.write((char *) &lE.term, sizeof(int64_t));
      wf.write((char *) lE.address, 4096);
      wf.write((char *) lE.data, 4096);
      wf.close();
  } else {
    return;
  }


  ifstream rf(raftLogPath.c_str(), ios::out | ios::binary);
  if(!rf.is_open())
    return;
  rf.seekg(0,ios::beg);
  int64_t curTerm;
  char voted_for[22];
  rf.read((char *) &curTerm, sizeof(int64_t));
  rf.read((char *) voted_for, sizeof(voted_for));
    string vf(voted_for);
    cout<<"on read Term in file: "<<curTerm<<"\nVote in file: "<<vf<<endl;
    cout<<"Log Entry\n";
    while (!rf.eof())
    {  
        LogEntry l; 
        rf.read((char *) &l, sizeof(LogEntry));
        cout<<"Term: "<<l.term<<" Address: "<<l.address<<" Value: "<<l.data<<endl;
        if (!rf.eof()) 
            break;
    }
    rf.close();


  //Any error check required?? 

}

//----------------------------------RaftInterfaceImp----------------------------------

class RaftInterfaceImpl final : public RaftAPI::Service {

  public: 
    //only one obj per registration?
    ServerRaft respRaft;
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
      string leaderId = request->leader_id();
      int64_t prevLogIndex = request->follower_log_idx();
      int64_t prevLogTerm = request->follower_log_term();

      ServerRaft respRaft;

      std::cout << "Recieved Append Entries from " <<  leaderId << " for term " << requestTerm <<
        ", prevLogIndex: " << prevLogIndex << " and prevLogTerm: " << prevLogTerm << std::endl;

      ServerRaft::last_comm_time = respRaft.get_time();

      if (requestTerm > respRaft.curTerm) {
        vote_lock.lock();
        char notvoted[22]=NOTVOTED;
        respRaft.update_term_and_voted_for(requestTerm, notvoted);
        respRaft.state = FOLLOWER;
        respRaft.current_leader_id = leaderId;
        vote_lock.unlock();
        cout << "\n1";
      }
  // check on valid term
      if(requestTerm < respRaft.curTerm) {
        reply->set_term(respRaft.curTerm);
        reply->set_success(false);
        cout<<"req term "<<requestTerm<<" less than cur term "<<respRaft.curTerm<< "\n";
        return Status::OK;
      } else {

        // Same term now update logs... 
        respRaft.state = FOLLOWER;
        cout << "\n2---";
        reply->set_term(respRaft.curTerm);
        reply->set_success(true);
      }

       log_lock.lock();
      // if prevlogindex is more than our last index, or term on prev log index is not same
      if(prevLogIndex >= (int64_t)raft_log.size() || (prevLogIndex >= 0 && raft_log[prevLogIndex].term != prevLogTerm)) {
        reply->set_success(false);
        cout<<"set false for if condition\n";
	      log_lock.unlock();
        return Status::OK;
      }

      // deleting entries after index with same term

      //TODO: binary write logic --> should be same trunctate
      if(prevLogIndex+1 < raft_log.size()) {
        cout << "\n inside truncate"; 
        truncate(respRaft.raftLogPath.c_str(), LOG_OFFSET + ((prevLogIndex + 1) * ENTRY_SIZE));
        raft_log.erase(raft_log.begin()+prevLogIndex+1, raft_log.end());
      }

      if(request->entries().size() > 0) {
        int64_t entryTerm = request->entries(0).term();
        if (raft_log[prevLogIndex+1].term != entryTerm) {
          raft_log.erase(raft_log.begin()+prevLogIndex+1, raft_log.end());
        }
      }

      struct LogEntry newEntry;
      //run a loop, keep on appending entries from WriteRequest
      for (int i = 0; i < request->entries().size(); i++) { 
        cout << "\n inside for---" ;
        newEntry.term = request->entries(i).term(); 

        cout << "\n-----" << request->entries(i).address().c_str();
        cout << "\n-----" << request->entries(i).data();

        memcpy(newEntry.address, request->entries(i).address().c_str(), 4096);
        memcpy(newEntry.data, request->entries(i).data().c_str(), 4096);
        respRaft.write_entry_to_log(newEntry);
        raft_log.push_back(newEntry);
      }
      log_lock.unlock();

      int64_t leaderCommitIdx = request->leader_commit();
      cout << "\n leaderCommitIdx ---- " << leaderCommitIdx;
      cout << "\n followerCommitIdx ---- " << ServerRaft::commit_index;
      if (leaderCommitIdx >  ServerRaft::commit_index) { //comparison should be with commit index
        int64_t new_commit_index = std::min(leaderCommitIdx, (int64_t)(raft_log.size()-1));
        cout << "\nhere---";
        // Apply the log entries to LevelDB --> madhu help
        // apply_entries(raspRaft.commit_index + 1, new_commit_index);

        ServerRaft::commit_index = new_commit_index;
      }


    reply->set_success(true);
    return Status::OK;
  }
};
