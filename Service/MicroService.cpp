
#include "MicroService.h"
#include "Json.hpp"

namespace elastos {

extern "C" {

MicroService* CreateService(const char* path, const char* publicKey)

{
    return new MicroService(path, publicKey);
}

void DestroyService(MicroService* service)
{
    if (service) {
        delete service;
    }
}

}

void MicroService::Init()
{
    mTransmitter = Transmitter::Instance(mPath, mPublicKey, this);
}

int MicroService::Start()
{
    if (mTransmitter.get() == NULL) return -1;
    printf("Service start!\n");

    int ret = mTransmitter->Start();
    if (ret != 0) return ret;

    std::string humanCode = mTransmitter->GetDid();
    printf("service did: %s\n", humanCode.c_str());

    return 0;
}

int MicroService::Stop()
{
    if (mTransmitter.get() == NULL) return -1;
    printf("Service stop!\n");

    return mTransmitter->Stop();
}

int MicroService::HandleMessage(const std::string& did, const std::string& msg)
{
    printf("MicroService handle did: %s message: %s\n", did.c_str(), msg.c_str());

    Json json = Json::parse(msg);
    std::string type = json["type"];
    std::string content = json["content"];

    Json respJson;
    if (!type.compare("friendRequest")) {
        respJson["type"] = "acceptFriend";
        respJson["content"] = "";
        mTransmitter->SendMessage(did, respJson.dump());
    }
    else if (!type.compare("textMsg")) {
        respJson["type"] = type;
        respJson["content"] = "hello";
        mTransmitter->SendMessage(did, respJson.dump());
    }

    return 0;
}

std::shared_ptr<std::vector<uint8_t>> MicroService::HandleData(int type, const std::vector<uint8_t>& data)
{
    if (mHandler.get() == nullptr) {
        printf("data handler is null\n");
        return std::shared_ptr<std::vector<uint8_t>>();
    }

    switch (type) {
    case OperationType_Encrypt:
        return mHandler->EncryptData(data);
    case OperationType_Decrypt:
        return mHandler->DecryptData(data);
    case OperationType_Sign:
        return mHandler->SignData(data);
    }

    return std::shared_ptr<std::vector<uint8_t>>();
}

void MicroService::SetDataHandler(std::shared_ptr<DataHandler>& handler)
{
    mHandler = handler;
}

}
