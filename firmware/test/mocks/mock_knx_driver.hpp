#pragma once
#include "src/drivers/knx/IKnxDriver.hpp"
#include <map>
#include <functional>

struct MockIKnxDriver : public drivers::knx::IKnxDriver
{
    int lastKoNumber = -1;
    std::function<void(GroupObject&)> lastCallback;
    std::map<int, std::function<void(GroupObject&)>> callbacks;

    bool init() override { return true; }
    std::string getApplicationVersion() override { return "1.0.0"; }

    drivers::knx::KnxConfig &getKnxConfig() override {
        static drivers::knx::KnxConfig *cfg = nullptr;
        return *cfg;
    }

    void setProgmodeChangeCallback(std::function<void(bool)> callback) override {}

    void register_cb(int ko_number, std::function<void(GroupObject&)> cb) override {
        lastKoNumber = ko_number;
        lastCallback = cb;
        callbacks[ko_number] = cb;
    }

    void toggleProgMode() override {}

    void simulateKnxEvent(int ko, int value, int dpt) {
        auto it = callbacks.find(ko);
        if (it != callbacks.end()) {
            GroupObject go;
            go.setValue(value);
            it->second(go);
        }
    }
};
