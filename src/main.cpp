#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include "rest_router.hpp"

int main() {
    Net::Address addr(Net::Ipv4::any(), Net::Port(8080));

    auto opts = Net::Http::Endpoint::options().threads(1);
    Net::Http::Endpoint server(addr);
    server.init(opts);

    server.setHandler(get_router().handler());
    server.serve();

	return 0;
}
