/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */

#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <stdexcept>
#include <iostream>
#include <string> // std::string, std::stoi
#include "rest_router.hpp"

char *main_argv2 = nullptr;

int main(int argc, char *argv[]) {

    uint16_t port = 8080;
	if (argc > 1){
		std::string szPort = argv[1];
		port = std::stoi(szPort, nullptr, 10);
	}

	if (argc > 2){
		main_argv2 = argv[2];
	}

    Net::Ipv4 ip = Net::Ipv4::any();

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

char * main_get_argv2(){
	return main_argv2;
}
