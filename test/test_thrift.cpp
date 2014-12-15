// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Something.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <fibio/thrift.hpp>
#include <fibio/fiberize.hpp>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::Test;


int count=0;
class SomethingHandler : virtual public SomethingIf {
public:
    SomethingHandler() {
        // Your initialization goes here
    }
    
    int32_t mul10(const int32_t n) {
        // Your implementation goes here
        count++;
        return n*10;
    }
};

std::unique_ptr<TFibioTCPServer> svr;
std::unique_ptr<fibio::barrier> bar;

void thrift_server() {
    shared_ptr<SomethingHandler> handler(new SomethingHandler());
    shared_ptr<TProcessor> processor(new SomethingProcessor(handler));
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    svr.reset(new TFibioTCPServer(processor,
                                  "39090",
                                  protocolFactory));
    bar->wait();
    svr->serve();
}

void thrift_client() {
    bar->wait();
    boost::shared_ptr<TTransport> transport(new TFibioTCPTransport("localhost:39090"));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    SomethingClient client(protocol);
    transport->open();
    assert(client.mul10(8)==80);
    assert(client.mul10(12)==120);
    assert(client.mul10(42)==420);
    transport->close();
    svr->stop();
}

int fibio::main(int argc, char **argv) {
    bar.reset(new fibio::barrier(2));
    fibio::fiber_group g;
    g.create_fiber(thrift_server);
    g.create_fiber(thrift_client);
    g.join_all();
    assert(count==3);
    std::cout << "main_fiber exiting" << std::endl;
    return 0;
}

