
#include "MicroService.h"

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

    return mTransmitter->Start();
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
    return 0;
}

}
