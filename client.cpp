using namespace std;
#include "./client.h"
Client::Client(std::shared_ptr<Channel> channel) : stub_(DistributeddBRaft::NewStub(channel)){}

int Client::Get(const std::string& key) {
    ReadReq request;
    Response reply;
    request.set_key(key);
    ClientContext context;
    Status status = stub_->Get(&context, request, &reply);
    if(status.ok()){
      std::cout << "Response data: " << reply.value() << endl;
    } else return reply.error_code();

}
int Client::Put(const std::string& key, const std::string& val) {
    WriteReq request;
    Response reply;
    request.set_key(key);
    request.set_value(val);
    ClientContext context;
    Status status = stub_->Put(&context, request, &reply);
    if(status.ok()){
      std::cout << "Response data: " << reply.value() << endl;
    } else return reply.error_code();

}


int main(int argc, char* argv[]) {
  string db_address = "0.0.0.0:50051";  // target address & port to send grpc requests to.
  Client *c = new Client(grpc::CreateChannel(db_address, grpc::InsecureChannelCredentials()));
  c->Put("name", "val1");
  c->Get("name1");
  c->Put("name1", "val2");
  c->Get("name1");
  c->Put("name", "val3");
  c->Get("name");
  return 0;
}


