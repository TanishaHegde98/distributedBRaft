using namespace std;
#include "./client.h"

Client::Client(std::shared_ptr<Channel> channel) : stub_(DistributeddBRaft::NewStub(channel)){}
int leader;
string leaderAddress = "";
int Client::Get(const std::string& key) {
    cout << "Arrived in Get" << endl;
    ReadReq request;
    Response reply;
    request.set_key(key);
    ClientContext context;
    Status status = stub_->Get(&context, request, &reply);
    cout << "Status:" << status.ok() << endl;
    if(status.ok()){
      cout << "Reply Status:" << reply.return_code() << endl;
      if(reply.return_code() == DB_SUCCESS){
        std::cout << "DB Get Success " << endl;
        std::cout << "Response data: " << reply.value() << endl;
        return reply.return_code();
      } else if(reply.return_code() == NOT_PRIMARY){ 
        std::cout << "Reached a Follower" << endl; 
        leaderAddress = reply.current_leader();
        return reply.return_code();
      } 
      std::cout << "Response data: " << reply.value() << endl;
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
        std::cout << "DB Put Success " << endl;
        return reply.return_code();
      } else if(reply.return_code() == NOT_PRIMARY){ 
        std::cout << "Reached a Follower" << endl; 
        leaderAddress = reply.current_leader();
        return reply.return_code();
      } 
      std::cout << "Response data: " << reply.value() << endl;
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
    //auto request_start_time = std::chrono::system_clock::now().time_since_epoch();
    uint64_t request_start_time = get_time();
    while (result != DB_SUCCESS) {
        // Wait some time between sending requests
       
        if (!first_try) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        first_try = false;
        
        cout << "leaderAddress : " << leaderAddress << endl;
        string serverAddress;
        if (leaderAddress != "") {
          cout << "inside" << endl;
          serverAddress = leaderAddress;
        } else {
          serverAddress =  dbServers[leader];
        }
        //Client *c = dbServerArr[leader];
        cout << "Trying for server: " << serverAddress << endl;
        
        Client *c = new Client(grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()));
        if(val.empty()){
          result = c->Get(key);
          cout << "Result after Get:" << result << endl;
        } else {
          result = c->Put(key, val);
        }

        // If we couldn't communicate with the given server, increment the primary
        if (result == NOT_PRIMARY || result == -1) {
            leader = (leader + 1) % dbServers.size();
            cout<<"Leader increment: "<<leader;
        }
        std::cout << "Leader:" << leader << ": " << ", Result: " << result << std::endl;
        std::cout << "Time:" << get_time() - request_start_time << endl;
    }
    
    return leader;
}

int main(int argc, char** argv) {
  if(argc < 2){
        cout<<"Usage: ./client <servers_file>\n";
        exit(0);
  }
    std::string filename = argv[1];
    cout << "Before process server" << endl;
    std::vector<std::string> dbServers;
    process_server_file(dbServers, filename);
    leader = 0;
    string key = "tani";
    string val = "hegde";
    leader = do_op(dbServers,key,val);
    //leader = do_op(dbServers, key, val);
    cout << "Leader in main" << leader << endl;
    cout << "Back into main" << endl;
    key = "madhu";
    val = "nij";
    //cout << "Trying for put" << endl;
    leader = do_op(dbServers, key, val);
    //leader = do_op(dbServers,key);

  return 0;
}



