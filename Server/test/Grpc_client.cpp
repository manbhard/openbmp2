/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <grpc++/grpc++.h>
#include <openbmp.grpc.pb.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using openbmp::Ip;
using openbmp::Response;
using openbmp::OPENBMPService;

class OPENBMPClient {
public:
    OPENBMPClient(std::shared_ptr<Channel> channel)
            : stub_(OPENBMPService::NewStub(channel)) {}

    //Disable Ip request
    std::string DisablePeer(const std::string& peer_ip) {
        Ip peer;
        peer.set_ip(peer_ip);
        Response response;

        ClientContext context;
        Status status = stub_->DisablePeer(&context, peer, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    //Enable Ip request
    std::string EnablePeer(const std::string& peer_ip) {
        Ip peer;
        peer.set_ip(peer_ip);
        Response response;

        ClientContext context;
        Status status = stub_->EnablePeer(&context, peer, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    std::string DisableRouter(const std::string& router_ip) {
        Ip router;
        router.set_ip(router_ip);
        Response response;

        ClientContext context;
        Status status = stub_->DisablePeer(&context, router, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    //Enable Ip request
    std::string EnableRouter(const std::string& router_ip) {
        Ip router;
        router.set_ip(router_ip);
        Response response;

        ClientContext context;
        Status status = stub_->EnableRouter(&context, router, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<OPENBMPService::Stub> stub_;
};

void usage() {
    printf("syntax: openbmpd_client grpc_server_ip:port [secure] [test]\n");
    printf("\nTESTS:\n\n");
    printf("    disable_peer    <peer_ip>       Block a specific peer\n");
    printf("    enable_peer     <peer_ip>       Unblock a specific peer\n");
    printf("    disable_router  <router_ip>     Disable a specific BMP router\n");
    printf("    enable_router   <router_ip>     Enable a previously disabled BMP router\n");
}


int main(int argc, char** argv) {
    if (argc <= 2) {
        printf("Incorrect number of args:");
        usage();
        exit(1);
    }

    std::shared_ptr<Channel> channel = NULL;

    int test_pos = 2;

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051).
    if (!strcmp(argv[2], "secure")) {
        std::ifstream tfile;
        tfile.open("client.crt");
        std::stringstream cert;
        cert << tfile.rdbuf();
        tfile.close();

        tfile.open("client.key");
        std::stringstream key;
        key << tfile.rdbuf();
        tfile.close();

        tfile.open("ca.crt");
        std::stringstream root;
        root << tfile.rdbuf();
        tfile.close();

        grpc::SslCredentialsOptions opts =
                {
                        root.str(),
                        key.str(),
                        cert.str()
                };

        channel = grpc::CreateChannel(argv[1], grpc::SslCredentials(opts));
        ++test_pos;
    } else {
        channel = grpc::CreateChannel(argv[1], grpc::InsecureChannelCredentials());
    }

    OPENBMPClient openbmp_client(channel);

    if (!strcmp(argv[test_pos], "disable_peer") && argc > (test_pos + 1)) {
        std::string peer_ip = argv[test_pos + 1];
        std::string reply = openbmp_client.DisablePeer(peer_ip);
        std::cout << "Response received: " << reply << std::endl;
    } else if (!strcmp(argv[test_pos], "enable_peer") && argc > (test_pos + 1)) {
        std::string peer_ip = argv[test_pos + 1];
        std::string reply = openbmp_client.EnablePeer(peer_ip);
        std::cout << "Response received: " << reply << std::endl;
    } else if (!strcmp(argv[test_pos], "disable_router") && argc > (test_pos + 1)) {
        std::string router_ip = argv[test_pos + 1];
        std::string reply = openbmp_client.DisableRouter(router_ip);
        std::cout << "Response received: " << reply << std::endl;
    } else if (!strcmp(argv[test_pos], "enable_router") && argc > (test_pos + 1)) {
        std::string router_ip = argv[test_pos + 1];
        std::string reply = openbmp_client.EnableRouter(router_ip);
        std::cout << "Response received: " << reply << std::endl;
    } else {
        printf("Invalid test '%s'\n", argv[1]);
        usage();
        exit(1);
    }
    return 0;
}

