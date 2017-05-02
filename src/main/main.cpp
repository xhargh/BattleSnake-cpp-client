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

char * main_get_argv2(){
	return main_argv2;
}


int main(int argc, char *argv[]) {

    uint16_t port = 8080;
    if (argc > 1){
        std::string szPort = argv[1];
        std::cout << "Got port " << szPort << std::endl;
        port = std::stoi(szPort, nullptr, 10);
    }
    else {
        std::cout <<"Usage: " << argv[0] << " <PORT> " << std::endl;
        std::cout <<"  If PORT is not specified, a default of 8080 is used." << std::endl;
    }

    if (argc > 2){
        //main_argv2 is used by the C snakes, since there are several possible.
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


