/*
 * Copyright (c) 2013-2018 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef OPENBMP_GRPC_H
#define OPENBMP_GRPC_H

#include "Config.h"
#include "Logger.h"
#include <grpc++/grpc++.h>
#include "client_thread.h"
#include <vector>

using grpc::Server;

class Grpc {
public:
    /***********************************************************************//**
     * Constructor for class
     *
     * @param [in] logPtr    Class instance for the logger
     * @param [in] cfgPtr    Class instance for config
     ***********************************************************************/
    Grpc(Logger *logPtr, Config *cfgPtr, vector<ThreadMgmt *> *thr_list);

    /***********************************************************************//**
     * Destructor for class - free memory that we allocated
     ***********************************************************************/
    ~Grpc();

    /***********************************************************************//**
     * run method
     ***********************************************************************/
    void RunServer();

    vector<ThreadMgmt *> *thr_list;
    Config          *cfg;                               ///< Pointer to the config class

private:
    std::unique_ptr<Server> server;

    bool            debug;                              ///< debug flag to indicate debugging
    Logger          *logger;                            ///< Logging class pointer

    /***********************************************************************//**
     * Enable/Disable debug
     ***********************************************************************/
    void enableDebug();
    void disableDebug();
};
#endif //OPENBMP_GRPC_H
