using namespace std;

#include <iostream>
#include <fstream>

#include "./client.h"

Client::Client(std::shared_ptr<Channel> channel) : stub_(DistributeddBRaft::NewStub(channel)){}
int leader;
string leaderAddress = "";
int Client::Get(const std::string& key) {
    ReadReq request;
    Response reply;
    request.set_key(key);
    ClientContext context;
    Status status = stub_->Get(&context, request, &reply);
    if(status.ok()){
      if(reply.return_code() == DB_SUCCESS){
        std::cout << "----- Response value for key "<< key << " : "<< reply.value() << " -----"<< endl;
        return reply.return_code();
      } else if(reply.return_code() == NOT_PRIMARY){ 
        std::cout << "Reached a Follower.Trying again with leader..." << endl; 
        leaderAddress = reply.current_leader();
        return reply.return_code();
      } else {
        leaderAddress = "";
      }
      return reply.error_code();
    } else {
      return -1;
    }
}
int Client::Put(const std::string& key, const std::string& val) {
    WriteReq request;
    Response reply;
    request.set_key(key);
    request.set_value(val);
    ClientContext context;
    Status status = stub_->Put(&context, request, &reply);
    if(status.ok()){
      if(reply.return_code() == DB_SUCCESS){
        std::cout << "----- Successfully written data ("<< key << ", " << val << ") -----" << endl;
        return reply.return_code();
      } else if(reply.return_code() == NOT_PRIMARY){ 
        leaderAddress = reply.current_leader();
        std::cout << "Reached a Follower...Trying again with leader "<< leaderAddress << endl;
        return reply.return_code();
      } else {
        leaderAddress = "";
      }
      return reply.error_code();
    } else {
      return -1;
    } 
}

 void process_server_file(std::vector<std::string> &dbServers, string filename){
    std::ifstream file(filename);
    std::string line;
    if (file.is_open()) {
    while(std::getline(file, line)) {
      dbServers.push_back(line);
    }
    file.close();
  }
 }
 uint64_t get_time(){
  
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}


  int do_op(std::vector<std::string> dbServers, const std::string& key, const std::string& val) {
    bool first_try = true;
    int result = -1, retry = 1;
    while (result != DB_SUCCESS) {
        // Wait some time between sending requests
        if (!first_try) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        first_try = false;
        
        string serverAddress = (leaderAddress != "") ? leaderAddress : dbServers[leader];

        ofstream wf ; // outs is an output stream of iostream class
        wf.open("serverLeader.txt") ; // connect outs to file outFile
        wf << serverAddress;
        wf.close () ;    // closing the output file stream

        Client *c = new Client(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
        if(val.empty()){
          cout << "Sending Get to " << serverAddress << endl;
          result = c->Get(key);
        } else {
          cout << "Sending Put to " << serverAddress << endl;
          result = c->Put(key, val);
        }

        // If we couldn't communicate with the given server, increment the primary
        if (result == NOT_PRIMARY || result == -1) {
            leader = (leader + 1) % dbServers.size();
            leaderAddress = "";
        }
    }
    
    return leader;
}

int main(int argc, char** argv) {
  if(argc < 2){
        cout<<"Usage: ./client <servers_file>\n";
        exit(0);
  }
    std::string filename = argv[1];
    std::vector<std::string> dbServers;
    process_server_file(dbServers, filename);
    leader = 0;

    ifstream rf("serverLeader.txt");
    if ( rf.is_open ()){
      while (getline ( rf , leaderAddress)) {
        cout << "leaderAddress : "  << leaderAddress;
      }
    }
    rf.close();
    

    // string key = "Sample key 1";
    // string val = "sample value 1";
    // leader = do_op(dbServers,key,val);

    // key = "Sample key 2";
    // val = "Sample key 2";
    // leader = do_op(dbServers, key, val);

    // key = "Sample key 1";
    // val = "updated value 1";
    // leader = do_op(dbServers,key,val);

    // key = "Sample key 1";
    // leader = do_op(dbServers,key, "");

    // key = "Sample key 3";
    // leader = do_op(dbServers,key, "");

    for(int i=0; i< 300; i++){
      string key="key";
      string val = "value";
      key+=to_string(i);
      val+=to_string(i);
      leader = do_op(dbServers, key, val);
      // cout<<"finished: "<<i<<endl;
    }

    //Read from db
    for(int i=0; i< 300; i++){
      string key="key";
      key+=to_string(i);
      string val="";
      leader = do_op(dbServers, key, val);
      //cout<<"Key: "<<key<<" val: "<<val<<endl;
    }
  return 0;

  return 0;
}



