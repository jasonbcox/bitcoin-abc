// Copyright (c) 2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <interface/node.h>

#include <chainparams.h>
#include <init.h>
#include <interface/handler.h>
#include <scheduler.h>
#include <ui_interface.h>
#include <util.h>
#include <warnings.h>

#include <boost/thread/thread.hpp>

class Config;
class HTTPRPCRequestProcessor;

namespace interface {
namespace {

    class NodeImpl : public Node {
        void parseParameters(int argc, const char *const argv[]) override {
            gArgs.ParseParameters(argc, argv);
        }
        void readConfigFile(const std::string &conf_path) override {
            gArgs.ReadConfigFile(conf_path);
        }
        void selectParams(const std::string &network) override {
            SelectParams(network);
        }
        void initLogging() override { InitLogging(); }
        void initParameterInteraction() override { InitParameterInteraction(); }
        std::string getWarnings(const std::string &type) override {
            return GetWarnings(type);
        }
        bool baseInitialize(Config &config, RPCServer &rpcServer) override {
            return AppInitBasicSetup() &&
                   AppInitParameterInteraction(config, rpcServer) &&
                   AppInitSanityChecks() && AppInitLockDataDirectory();
        }
        bool appInitMain(Config &config, HTTPRPCRequestProcessor &httpRPCRequestProcessor) override {
            return AppInitMain(config, httpRPCRequestProcessor);
        }
        void appShutdown() override {
            Interrupt();
            Shutdown();
        }
        void startShutdown() override { StartShutdown(); }
        std::unique_ptr<Handler> handleInitMessage(InitMessageFn fn) override {
            return MakeHandler(::uiInterface.InitMessage.connect(fn));
        }
    };

} // namespace

std::unique_ptr<Node> MakeNode() {
    return MakeUnique<NodeImpl>();
}

} // namespace interface
