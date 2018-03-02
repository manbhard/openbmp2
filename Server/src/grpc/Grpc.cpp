/*
 * Copyright (c) 2013-2015 Cisco Systems, Inc. and others.  All rights reserved.
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
#include <sys/socket.h>

#include "Grpc.h"
#include <openbmp.grpc.pb.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;
using openbmp::Ip;
using openbmp::Response;
using openbmp::OPENBMPService;

// Logic and data behind the server's behavior.
class OPENBMPServiceImpl final : public OPENBMPService::Service {
public:
    Grpc *grpc;

private:
    Status DisableRouter(ServerContext* context, const Ip* router,
                       Response* reply) override {
        string grpc_response;

        bool not_found = true;
        for (size_t i=0; i < grpc->cfg->blocked_routers.size(); i++) {
            if (grpc->cfg->blocked_routers.at(i) == router->ip()) {
                not_found = false;
                break;
            }
        }

        if (not_found) {
            grpc->cfg->blocked_routers.push_back(router->ip());
        }

        for (size_t i=0; i < grpc->thr_list->size(); i++) {
            if (strncmp(grpc->thr_list->at(i)->client.c_ip, router->ip().c_str(), strlen(router->ip().c_str())) == 0) {
                int error = shutdown(grpc->thr_list->at(i)->client.c_sock, SHUT_RDWR);
                if (error) {
                    grpc_response = "Failed to close socket for router: " + router->ip() + ", error: " + strerror(error);
                    reply->set_message(grpc_response.c_str());
                    return Status(StatusCode::INTERNAL, reply->message());
                }
                grpc_response = "Disabled router: " + router->ip();
                reply->set_message(grpc_response.c_str());
                return Status(StatusCode::OK, reply->message());
            }
        }
        grpc_response = "Router: " + router->ip() + " does not exist";
        reply->set_message(grpc_response.c_str());
        return Status(StatusCode::OK, reply->message());
    }

    Status EnableRouter(ServerContext* context, const Ip* router,
                       Response* reply) override {
        string grpc_response;

        for (size_t i=0; i < grpc->cfg->blocked_routers.size(); i++) {
            if (grpc->cfg->blocked_routers.at(i) == router->ip()) {
                grpc->cfg->blocked_routers.erase(grpc->cfg->blocked_routers.begin() + i);
                grpc_response = "Unblocked router: " + router->ip();
                reply->set_message(grpc_response.c_str());
                return Status(StatusCode::OK, reply->message());
            }
        }
        reply->set_message("Router is not blocked");
        return Status(StatusCode::OK, reply->message());
    }

    Status DisablePeer(ServerContext* context, const Ip* peer,
                         Response* reply) override {
        string grpc_response;

        bool not_found = true;
        for (size_t i=0; i < grpc->cfg->blocked_peers.size(); i++) {
            if (grpc->cfg->blocked_peers.at(i) == peer->ip()) {
                not_found = false;
                break;
            }
        }

        if (not_found) {
            grpc->cfg->blocked_peers.push_back(peer->ip());
        }

        grpc_response = "Peer: " + peer->ip() + " is blocked";
        reply->set_message(grpc_response.c_str());
        return Status(StatusCode::OK, reply->message());
    }

    Status EnablePeer(ServerContext* context, const Ip* peer,
                        Response* reply) override {
        string grpc_response;

        for (size_t i=0; i < grpc->cfg->blocked_peers.size(); i++) {
            if (grpc->cfg->blocked_peers.at(i) == peer->ip()) {
                grpc->cfg->blocked_peers.erase(grpc->cfg->blocked_peers.begin() + i);
                grpc_response = "Unblocked peer: " + peer->ip();
                reply->set_message(grpc_response.c_str());
                return Status(StatusCode::OK, reply->message());
            }
        }

        reply->set_message("Peer is not blocked");
        return Status(StatusCode::OK, reply->message());
    }

};


Grpc::Grpc(Logger *logPtr, Config *cfgPtr, vector<ThreadMgmt *> *thr_list) {
    // initialize class pointers
    logger = logPtr;
    cfg = cfgPtr;
    this->thr_list = thr_list;
}

/***********************************************************************//**
 * Destructor for class - free memory that we allocated
 ***********************************************************************/
Grpc::~Grpc() {
    server->Shutdown();
}

void Grpc::RunServer() {
    std::string server_address("0.0.0.0:" + std::to_string(cfg->interfaceConfig.grpc_port));
    OPENBMPServiceImpl service;
    service.grpc = this;
    ServerBuilder builder;

    if (cfg->debug_grpc)
        enableDebug();

    if (cfg->interfaceConfig.secure) {
        std::ifstream tfile;
        tfile.open("server.crt");
        if (!tfile.is_open()) {
            LOG_ERR("Error opening file server.crt");
            exit(1);
        }
        std::stringstream cert;
        cert << tfile.rdbuf();
        tfile.close();

        tfile.open("server.key");
        if (!tfile.is_open()) {
            LOG_ERR("Error opening file server.key");
            exit(1);
        }
        std::stringstream key;
        key << tfile.rdbuf();
        tfile.close();

        tfile.open("ca.crt");
        if (!tfile.is_open()) {
            LOG_ERR("Error opening file ca.crt");
            exit(1);
        }
        std::stringstream root;
        root << tfile.rdbuf();
        tfile.close();

        grpc::SslServerCredentialsOptions::PemKeyCertPair keycert =
                {
                        key.str(),
                        cert.str()
                };

        grpc::SslServerCredentialsOptions sslOps;
        sslOps.pem_root_certs = root.str();
        sslOps.pem_key_cert_pairs.push_back(keycert);

        builder.AddListeningPort(server_address, grpc::SslServerCredentials(sslOps));
    } else {
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    }

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    server = builder.BuildAndStart();
    LOG_INFO("Server listening on %s", server_address.c_str());

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

/***********************************************************************//**
 * Enable/Disable debug
 ***********************************************************************/
void Grpc::enableDebug() {
    debug = true;
}

void Grpc::disableDebug() {
    debug = false;
}
