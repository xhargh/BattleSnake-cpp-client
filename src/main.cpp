#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <stdexcept>
#include "rest_router.hpp"

int main() {
    Net::Ipv4 ip = Net::Ipv4::any();
    uint16_t port = 8057;

    std::cout << "Serving Battlesnake client on " << ip.toString() << ":" << port << std::endl;

    Net::Address addr(ip, Net::Port(port));

    auto opts = Net::Http::Endpoint::options().threads(1).flags(Net::Tcp::Options::ReuseAddr);
    Net::Http::Endpoint server(addr);
    server.init(opts);

    server.setHandler(get_router().handler());

    try {
        server.serve();
    } catch (std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl << "Another instance of this client running?" << std::endl;
    }
	return 0;
}
