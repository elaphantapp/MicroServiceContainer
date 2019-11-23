
#include "Transmitter.h"
#include <sstream>
#include "ContactDebug.hpp"

namespace elastos {

std::shared_ptr<Transmitter> Transmitter::s_instance;


/**************  Transmitter::Listener **************/
std::shared_ptr<std::vector<uint8_t>> Transmitter::Listener::onAcquire(const AcquireArgs& request)
{
    std::shared_ptr<std::vector<uint8_t>> response;

    switch(request.type) {
    case ElaphantContact::Listener::AcquireType::PublicKey:
        response = std::make_shared<std::vector<uint8_t>>(
                    mTransmitter->mPublicKey.begin(), mTransmitter->mPublicKey.end());
        break;
    case ElaphantContact::Listener::AcquireType::EncryptData:
        response = mTransmitter->HandleData(IMicroService::OperationType_Encrypt, request.data);
        break;
    case ElaphantContact::Listener::AcquireType::DecryptData:
        response = mTransmitter->HandleData(IMicroService::OperationType_Decrypt, request.data);
        break;
    case ElaphantContact::Listener::AcquireType::DidPropAppId:
    {
        std::string appId = "DC92DEC59082610D1D4698F42965381EBBC4EF7DBDA08E4B3894D530608A64AA"
                            "A65BB82A170FBE16F04B2AF7B25D88350F86F58A7C1F55CC29993B4C4C29E405";
        response = std::make_shared<std::vector<uint8_t>>(appId.begin(), appId.end());
        break;
    }
    case ElaphantContact::Listener::AcquireType::DidAgentAuthHeader:
    {
        std::string header = mTransmitter->GetAgentAuthHeader();
        printf("auth header: %s\n", header.c_str());
        response = std::make_shared<std::vector<uint8_t>>(header.begin(), header.end());
        break;
    }
    case ElaphantContact::Listener::AcquireType::SignData:
    {
        printf("request sign data\n");
        response = mTransmitter->HandleData(IMicroService::OperationType_Sign, request.data);
        break;
    }

    }

    return response;
}

void Transmitter::Listener::onEvent(EventArgs& event)
{
    switch (event.type) {
    case ElaphantContact::Listener::EventType::StatusChanged:
    {
        auto statusEvent = dynamic_cast<ElaphantContact::Listener::StatusEvent*>(&event);
        HandleStatusChanged(event.humanCode, statusEvent->status);
        break;
    }
    case ElaphantContact::Listener::EventType::FriendRequest:
    {
        auto requestEvent = dynamic_cast<ElaphantContact::Listener::RequestEvent*>(&event);
        mTransmitter->HandleMessage(event.humanCode,
                "friendRequest", requestEvent->summary, requestEvent->summary.size());
        break;
    }
    case ElaphantContact::Listener::EventType::HumanInfoChanged:
    {
        auto infoEvent = dynamic_cast<ElaphantContact::Listener::InfoEvent*>(&event);
        std::string content = infoEvent->toString();
        mTransmitter->HandleMessage(event.humanCode, "infoChanged", content, content.size());
        break;
    }
    default:
        printf("Unprocessed event: %d", static_cast<int>(event.type));
        break;
    }
}

void Transmitter::Listener::onReceivedMessage(const std::string& humanCode,
                               ContactChannel channelType,
                               std::shared_ptr<ElaphantContact::Message> msgInfo)
{
    if (msgInfo->type == ElaphantContact::Message::Type::MsgText) {
        std::string content = msgInfo->data->toString();
        mTransmitter->HandleMessage(humanCode, "textMsg", content, content.size());
    }
    else {
        printf("Received message type: %s\n", std::to_string(static_cast<int>(msgInfo->type)).c_str());
    }
}

void Transmitter::Listener::onError(int errCode, const std::string& errStr, const std::string& ext)
{
    printf("contact error code: %d: %s, %s\n", errCode, errStr.c_str(), ext.c_str());
}

void Transmitter::Listener::HandleStatusChanged(const std::string& humanCode, elastos::HumanInfo::Status status)
{
    std::string type = "status";
    std::string content;
    switch(status) {
    case elastos::HumanInfo::Status::Invalid:
        content = "Invalid";
        break;
    case elastos::HumanInfo::Status::WaitForAccept:
        content = "WaitForAccept";
        break;
    case elastos::HumanInfo::Status::Offline:
        content = "Offline";
        break;
    case elastos::HumanInfo::Status::Online:
        content = "Online";
        break;
    case elastos::HumanInfo::Status::Removed:
        content = "Removed";
        break;
    default:
        content = "unknown";
        break;
    }

    mTransmitter->HandleMessage(humanCode, type, content, content.size());
}

/**************  Transmitter::DataListener **************/
void Transmitter::DataListener::onNotify(const std::string& humanCode,
                      ContactChannel channelType,
                      const std::string& dataId, int status)
{
    printf("Contact data notify: %s, dataId: %s, status: %s\n",
            humanCode.c_str(), dataId.c_str(), std::to_string(status).c_str());
}

int Transmitter::DataListener::onReadData(const std::string& humanCode,
                       ContactChannel channelType,
                       const std::string& dataId, uint64_t offset,
                       std::vector<uint8_t>& data)
{
    printf("Contact onReadData: %s, dataId: %s, offset: %lld\n",
            humanCode.c_str(), dataId.c_str(), offset);
    return 0;
}

int Transmitter::DataListener::onWriteData(const std::string& humanCode,
                        ContactChannel channelType,
                        const std::string& dataId, uint64_t offset,
                        const std::vector<uint8_t>& data)
{
    printf("Contact onWriteData: %s, dataId: %s, offset: %lld\n",
            humanCode.c_str(), dataId.c_str(), offset);
    return 0;
}


/**************  Transmitter **************/
std::shared_ptr<Transmitter> Transmitter::Instance(const std::string& path, const std::string& publicKey, IMicroService* service)
{
    if (s_instance.get() == nullptr) {
        s_instance = std::shared_ptr<Transmitter>(new Transmitter(service, publicKey));
        s_instance->Init(path);
    }

    return s_instance;
}

void Transmitter::Init(const std::string& path)
{
    ElaphantContact::Factory::SetLogLevel(7);
    ElaphantContact::Factory::SetLocalDataDir(path);

    mContact = ElaphantContact::Factory::Create();
    if (mContact.get() == NULL) {
        printf("ElaphantContact Factory Create failed!\n");
        return;
    }

    mListener = std::make_shared<Transmitter::Listener>(this);
    mDataListener = std::make_shared<Transmitter::DataListener>(this);
    mContact->setListener(mListener.get());
    mContact->setDataListener(mDataListener.get());
}

void Transmitter::HandleMessage(const std::string& humanId, const std::string& type, const std::string& content, int length)
{
    Json json;
    json["type"] = type;
    json["content"] = content;
    json["lenght"] = length;

    mService->HandleMessage(humanId, json.dump());
}

std::shared_ptr<std::vector<uint8_t>> Transmitter::HandleData(int type, const std::vector<uint8_t>& data)
{
    return mService->HandleData(type, data);
}

int Transmitter::Start()
{
    if (mContact.get() == NULL) return -1;

    printf("Transmitter start!\n");
    int ret = mContact->start();
    if (ret != 0) return ret;

    std::stringstream ss;
    ContactDebug::GetCachedDidProp(&ss);
    printf("cachedDidProp: %s\n", ss.str().c_str());
    mContact->syncInfoUploadToDidChain();

    return 0;
}

int Transmitter::Stop()
{
    if (mContact.get() == NULL) return -1;

    printf("Transmitter stop!\n");
    return mContact->stop();
}

int Transmitter::SendMessage(const std::string& did, const std::string& msg)
{
    printf("Transmitter SendMessage: %s\n", msg.c_str());
    Json json = Json::parse(msg);
    std::string type = json["type"];
    std::string content = json["content"];

    if (!type.compare("acceptFriend")) {
        mContact->acceptFriend(did);
    }
    else if (!type.compare("textMsg")) {
        auto msgInfo = ElaphantContact::MakeTextMessage(content);
        if(msgInfo == nullptr) {
            printf("Failed to make text message.");
            return -1;
        }

        auto ret = mContact->sendMessage(did.c_str(), ContactChannel::Carrier, msgInfo);
    }

    return 0;
}

std::string Transmitter::GetDid()
{
    auto userInfo = mContact->getUserInfo();
    if (userInfo == nullptr) {
        printf("getUserInfo failed!\n");
        return "";
    }

    std::string humanCode;
    userInfo->getHumanCode(humanCode);

    return humanCode;
}

int Transmitter::GetFriendList(std::stringstream* info)
{
    if (!info) return -1;

    return mContact->getFriendList(info);
}

std::string Transmitter::GetAgentAuthHeader()
{
    std::string appid = "org.elastos.microservice.test";
    std::string appkey = "b2gvzUM79yLhCbbGNWCuhSsGdqYhA7sS";
    std::string headerValue = "id=" + appid + ";time=77777;auth=" + appkey;

    return headerValue;
}

}
