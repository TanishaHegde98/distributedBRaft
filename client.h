#include <iostream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <grpc++/grpc++.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <errno.h>
#include <fstream> 
#include <chrono>
#include <ctime>
#include <thread> 
#define TIMEOUT (7000)
#include "globals.h"
//#include "wiscAFS.grpc.pb.h"
#include "distributeddBRaft.grpc.pb.h"
//#include "distributeddBRaft.pb.h"

using namespace std;
using distributeddBRaft::ReadReq;
using distributeddBRaft::WriteReq;
using distributeddBRaft::Response;
using distributeddBRaft::DistributeddBRaft;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::Status;


class Client{
public:
    Client(std::shared_ptr<Channel> channel);
    int Get(const std::string&);
    int Put(const std::string&, const std::string& val);
    //uint64_t Client::get_time();

private:
    std::unique_ptr< DistributeddBRaft::Stub> stub_;
};




