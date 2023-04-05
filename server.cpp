#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string>
//Should comment
#include <grpc++/grpc++.h>

#include "leveldb/db.h"

//Should comment
#include "distributeddBRaft.grpc.pb.h"
#include "serverRaft.cpp"

using namespace std;
using distributeddBRaft::ReadReq;
using distributeddBRaft::WriteReq;
using distributeddBRaft::Response;
using distributeddBRaft::DistributeddBRaft;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;

//Global object for serverRaft

ServerRaft raft;

class distributeddBRaftImpl final : public DistributeddBRaft::Service {
    public:
        Status Get(ServerContext* context, const ReadReq* request, Response* writer)  {
            cout << "In Get Client" << endl;
            leveldb::DB *db;
            leveldb::Options options;
            options.create_if_missing = true;
            int key_found = 0;
            // open
            std::cout << "Request key: " << request->key() << endl;
            std::string val;
            leveldb::Status dbStatus = leveldb::DB::Open(options, "/tmp/testdb", &db);
        
            if (!dbStatus.ok()) {
                std::cout << "DB Read Error" << endl;
                writer->set_error_code(-2);
            }
            leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                if(it->key().ToString() == request->key()){
                    key_found = 1;
                    val = it->value().ToString();
                    
                    
                }
            }
            if(key_found){
                if(val.empty()){
                    std::cout << "Value for key " << request->key() << " empty" << endl;
                } else {
                    std::cout << "Value: " << val << endl;
                }   
            } else {
                std::cout << "Key not found " << request->key() << endl;
            }
            delete it;
            delete db;
            writer->set_value(val);
            writer->set_error_code(0);
            return Status::OK;
        }

        Status Put(ServerContext* context, const WriteReq* request,Response* writer) override {
            cout << "In Put Client" << endl;
            leveldb::DB *db;
            leveldb::Options options;
            options.create_if_missing = true;
            // open
            int key_found = 0;
            std::string val;

            leveldb::Status dbStatus = leveldb::DB::Open(options, "/tmp/testdb", &db);
                        cout<< "\ndb:" << &db;
            if (!dbStatus.ok()) {
                std::cout << "DB Read Error" << endl;
                writer->set_error_code(-2);
            }
            std::string value = request->value();
            std::cout << "Request key: " << request->key() << endl;
            std::cout << "Value: " << value << endl;

            dbStatus = db->Put(leveldb::WriteOptions(), request->key(), value);
            if (!dbStatus.ok()) {
                std::cout << "Write Error" << endl;
                writer->set_error_code(-errno);
            } else {
            writer->set_error_code(0);
            writer->set_value(value);
            }
            delete db;
            return Status::OK;
        }
};
void RunServer(string listen_port, vector<string>other_servers) {
    std::string server_address(listen_port);
    distributeddBRaftImpl service;
    RaftInterfaceImpl raftService;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    builder.RegisterService(&raftService);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    
}

int main(int argc, char** argv) {
    //TODO: accept arguements , process serverFile, call runServer with different thread
    //  
    if(argc < 3){
        cout<<"too few arguements\n";
        cout<<"Usage: ./server <id> <listen_port> <servers_file> [is_primary]/n";
        exit(0);
    }

    if(argc == 5)
        raft.state = LEADER;
    else
        raft.state = FOLLOWER;
    //TODO: start as leader and make state as leader
    // only check log persistance
    // make changes to server put and get to reject
    // crate separate log files for each server
    // mylogserverid.log
    

    //TODO: change server_id type to string and use full_path
    ServerRaft::server_id = stoi(argv[1]);
    string server_ip(argv[1]);
    string port(argv[2]);
    string colon(":");
    string full_server_path = server_ip+colon+port;
    string filename = argv[3];

    raft.process_server_file(filename);
    
    raft.last_comm_time = raft.get_time() + 20000;
    for(auto o:ServerRaft::other_servers){
        cout<<o<<endl;
    }
    
    //RunServer(full_server_path,ServerRaft::other_servers);
    std::thread server_thread(RunServer, full_server_path, ServerRaft::other_servers);

    //server_thread.join();
    raft.handleHeartbeats();
    //TODO:
    // sr.read_raft_log()
    // sr.handle_heartbeats();
    // std::thread ldr_commit_thread(sr.commit_thread);
    return 0;
}