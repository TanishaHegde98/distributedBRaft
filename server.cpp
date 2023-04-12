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
#include <cstring>
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

#define NOTVOTED ""

string db_path = "/tmp/testdb";
ServerRaft raft;

bool db_put(string key, string value, int &errorCode, int &returnCode){

    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    // cout<<"Db path: "<<db_path<<endl;
    int key_found = 0;
    leveldb::Status dbStatus = leveldb::DB::Open(options, db_path, &db);
    assert(dbStatus.ok());
    if (!dbStatus.ok()) {
        std::cout << "DB Read Error" << endl;
        returnCode=DB_FAIL;
        errorCode=-1;
        delete db;
        return dbStatus.ok();
    }

    dbStatus = db->Put(leveldb::WriteOptions(), key, value);
    assert(dbStatus.ok());
    if (!dbStatus.ok()) {
        std::cout << "Write Error" << endl;
        returnCode=DB_FAIL;
        errorCode=-1;
    } else {
        // cout<<"db put success\n";
        errorCode=0;
        returnCode=DB_SUCCESS;
    }
    delete db;

    return dbStatus.ok();
}

class distributeddBRaftImpl final : public DistributeddBRaft::Service {
    public:
        Status Get(ServerContext* context, const ReadReq* request, Response* writer)  {
            cout << "------------------------------------------------" << endl;
            // cout << "Received Get request from client: get(" << request->key() << ")"<< endl;
            if(raft.state == FOLLOWER){
                writer->set_return_code(NOT_PRIMARY);
                writer->set_error_code(0);
                writer->set_current_leader(raft.current_leader_id);
                cout << "Redirecting to the leader" << endl;
                cout << "------------------------------------------------" << endl;
                return Status::OK;
            }
            leveldb::DB *db;
            leveldb::Options options;
            options.create_if_missing = true;
            int key_found = 0;
            // open
            // std::cout << "Request key: " << request->key() << endl;
            std::string val;
            leveldb::Status dbStatus = leveldb::DB::Open(options, db_path, &db);
        
            if (!dbStatus.ok()) {
                std::cout << "DB Read Error" << endl;
                writer->set_return_code(DB_FAIL);
                writer->set_error_code(-1);
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
            writer->set_return_code(DB_SUCCESS);
            cout << "------------------------------------------------" << endl;
            return Status::OK;
        }

        Status Put(ServerContext* context, const WriteReq* request,Response* writer) override {
            
            int entry_index;
            cout << "------------------------------------------------" << endl;
            cout << "Received Put request from client: put(" << request->key() 
                                <<  ", " << request->value() << ")" << endl;
            if(raft.state == FOLLOWER){
                writer->set_return_code(NOT_PRIMARY);
                writer->set_error_code(0);
                writer->set_current_leader(raft.current_leader_id);
                cout << "Redirecting to the leader" << endl;
                cout << "------------------------------------------------" << endl;
                return Status::OK;
            }
            // leveldb::DB *db;
            // leveldb::Options options;
            // options.create_if_missing = true;
            // // open
            // int key_found = 0;
            // std::string val;

            // leveldb::Status dbStatus = leveldb::DB::Open(options, "/tmp/testdb", &db);
            // assert(dbStatus.ok());
            // // cout<< "\ndb:" << &db;
            // if (!dbStatus.ok()) {
            //     std::cout << "DB Read Error" << endl;
            //     writer->set_return_code(DB_FAIL);
            //     writer->set_error_code(-1);
            // }
            // std::string value = request->value();

            // dbStatus = db->Put(leveldb::WriteOptions(), request->key(), value);
            // if (!dbStatus.ok()) {
            //     std::cout << "Write Error" << endl;
            //     writer->set_return_code(DB_FAIL);
            //     writer->set_error_code(-1);
            // } else {
            //     writer->set_error_code(0);
            //     writer->set_value(value);
            //     writer->set_return_code(DB_SUCCESS);
            // }
            // delete db;

            int errorCode=0, returnCode=0;
            bool dbStatus = db_put(request->key(), request->value(),errorCode,returnCode);
            if(!dbStatus){
                writer->set_return_code(returnCode);
                writer->set_error_code(errorCode);
                return Status::OK;
            }
            else{
                writer->set_return_code(returnCode);
                writer->set_error_code(errorCode);
                writer->set_value(request->value());
            }
            
            // cout << "before log write" << raft.get_time() << endl;

            LogEntry log_entry;
            log_entry.term = raft.curTerm;
            memcpy(log_entry.address, request->key().c_str(), 4096);
            memcpy(log_entry.data, request->value().c_str(), 4096);

            // Add the new entry to the log
            log_lock.lock();
            raft.write_entry_to_log(log_entry);
            raft.readonly_raft_log();
            raft_log.push_back(log_entry);
            entry_index = raft_log.size() - 1;
            log_lock.unlock();
            // cout << "after log write" << raft.get_time() << endl;

            // Wait for the update to be commited before returning to the client
            while (ServerRaft::commit_index < entry_index) {
                // Have we somehow been demoted from leader?
                // If so, forward the client to the new leader
                if (raft.state != LEADER) {
                    writer->set_return_code(-1);
                    writer->set_current_leader(raft.current_leader_id);
                    cout << "Redirecting to the leader" << endl;
                    cout << "------------------------------------------------" << endl;
                    return Status::OK;
                }
                ServerRaft::commit_index += 1;
                // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } 
            cout << "------------------------------------------------" << endl;           
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
    std::cout << "\nListening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    
}

int main(int argc, char** argv) {
    //TODO: accept arguements , process serverFile, call runServer with different thread
    //  
    if(argc < 3){
        cout<<"too few arguements\n";
        cout<<"Usage: ./server <id>:<listen_port> <servers_file> [is_primary]/n";
        exit(0);
    }

    if(argc == 4)
        raft.state = LEADER;
    else
        raft.state = FOLLOWER;
    //TODO: start as leader and make state as leader
    // only check log persistance
    // make changes to server put and get to reject
    // crate separate log files for each server
    // mylogserverid.log
    
    ServerRaft::server_id = stoi(argv[1]);
    string full_server_path = argv[1];
    string filename = argv[2];

    raft.process_server_file(filename);

    //will create raftLogPath too
    raft.set_serverId(full_server_path);

    //TODO: is this needed ?
    raft.read_raft_log();

    raft.last_comm_time = raft.get_time() + 20000;
    
    db_path+=ServerRaft::server_id;

    std::thread server_thread(RunServer, full_server_path, ServerRaft::other_servers);
    cout << "Other servers: ";
    for(auto o:ServerRaft::other_servers){
        cout << o << "  ";
    }
    raft.handleHeartbeats();

    return 0;
}