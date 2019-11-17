
#include "Transmitter.h"
#include <sstream>

namespace elastos {

Transmitter* Transmitter::s_instance = NULL;


/**************  Transmitter::SecurityListener **************/
std::string Transmitter::SecurityListener::onAcquirePublicKey()
{
    return mTransmitter->mPublicKey;
}

std::vector<uint8_t> Transmitter::SecurityListener::onEncryptData(const std::string& pubKey, const std::vector<uint8_t>& src)
{
    return src;
}

std::vector<uint8_t> Transmitter::SecurityListener::onDecryptData(const std::vector<uint8_t>& src)
{
    return src;
}

std::string Transmitter::SecurityListener::onAcquireDidPropAppId()
{
    return "";
}

std::string Transmitter::SecurityListener::onAcquireDidAgentAuthHeader()
{
    return "";
}

std::vector<uint8_t> Transmitter::SecurityListener::onSignData(const std::vector<uint8_t>& originData)
{
    assert(0);
    return std::vector<uint8_t>();
}


/**************  Transmitter::MessageListener **************/

void Transmitter::MessageListener::onStatusChanged(std::shared_ptr<elastos::UserInfo> userInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     elastos::UserInfo::Status status)
{
    HandleStatusChanged(userInfo.get(), status);
}

void Transmitter::MessageListener::onReceivedMessage(std::shared_ptr<elastos::HumanInfo> humanInfo,
                               elastos::MessageManager::ChannelType channelType,
                               const std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo)
{
    std::string humanCode;
    humanInfo->getHumanCode(humanCode);

    if (msgInfo->mType == elastos::MessageManager::MessageType::MsgText) {
        uint8_t* data = msgInfo->mPlainContent.data();
        std::string content(reinterpret_cast<const char*>(data));
        mTransmitter->HandleMessage(humanCode, "textMsg", content, content.size());
    }
}

void Transmitter::MessageListener::onSentMessage(int msgIndex, int errCode)
{

}

void Transmitter::MessageListener::onFriendRequest(std::shared_ptr<elastos::FriendInfo> friendInfo,
                             elastos::MessageManager::ChannelType channelType,
                             const std::string& summary)
{
    std::string humanCode;
    friendInfo->getHumanCode(humanCode);
    mTransmitter->HandleMessage(humanCode, "friendRequest", summary, summary.size());
}

void Transmitter::MessageListener::onFriendStatusChanged(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                   elastos::MessageManager::ChannelType channelType,
                                   elastos::FriendInfo::Status status)
{
    HandleStatusChanged(friendInfo.get(), status);
}

void Transmitter::MessageListener::onHumanInfoChanged(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                        elastos::MessageManager::ChannelType channelType)
{
    std::string humanCode;
    humanInfo->getHumanCode(humanCode);

    std::string content;
    humanInfo->serialize(content, true);

    mTransmitter->HandleMessage(humanCode, "infoChanged", content, content.size());
}

void Transmitter::MessageListener::HandleStatusChanged(HumanInfo* humanInfo, elastos::HumanInfo::Status status)
{
    std::string humanCode;
    humanInfo->getHumanCode(humanCode);

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


/**************  Transmitter **************/
std::shared_ptr<Transmitter> Transmitter::Instance(const std::string& path, const std::string& publicKey, IMicroService* service)
{
    if (s_instance == NULL) {
        s_instance = new Transmitter(service, publicKey);
        s_instance->Init(path);
    }

    return std::shared_ptr<Transmitter>(s_instance);
}

void Transmitter::Init(const std::string& path)
{
    Contact::Factory::SetLogLevel(7);
    Contact::Factory::SetLocalDataDir(path);

    mContact = Contact::Factory::Create();
    if (mContact.get() == NULL) {
        printf("Contact Factory Create failed!\n");
        return;
    }

    auto securityListener = std::make_shared<Transmitter::SecurityListener>(this);
    auto messageListener = std::make_shared<Transmitter::MessageListener>(this);
    mContact->setListener(securityListener, NULL, NULL, messageListener);
}

void Transmitter::HandleMessage(const std::string& humanId, const std::string& type, const std::string& content, int length)
{
    std::stringstream ss;
    ss << "{\"type\":\"" << type << "\", \"content\":\"" << content << "\", \"length\":" << length << "}";

    mService->HandleMessage(humanId, ss.str());
}

int Transmitter::Start()
{
    if (mContact.get() == NULL) return -1;

    printf("Transmitter start!\n");
    mContact->start();
    return 0;
}

int Transmitter::Stop()
{
    if (mContact.get() == NULL) return -1;

    printf("Transmitter stop!\n");
    mContact->stop();
    return 0;
}

int Transmitter::SendMessage(const std::string& did, const std::string& msg)
{
    printf("Transmitter SendMessage!\n");
    return 0;
}

std::string Transmitter::GetDid()
{
    if (!mContact->isStarted()) {
        printf("Please start transmitter first!\n");
        return "";
    }

    std::weak_ptr<elastos::UserManager> userManager = mContact->getUserManager();
    auto um = userManager.lock();
    if (um.get() == nullptr) {
        return "";
    }

    std::shared_ptr<elastos::UserInfo> userInfo;
    um->getUserInfo(userInfo);

    std::string humanCode;
    userInfo->getHumanCode(humanCode);

    return humanCode;
}

}
