
#ifndef _ELASTOS_TRANSMITTER_H__
#define _ELASTOS_TRANSMITTER_H__

#include <memory>
#include <string>
#include "IMicroService.h"
#include "Elastos.SDK.Contact.hpp"

namespace elastos {

class Transmitter {
private:
    class SecurityListener : public elastos::SecurityManager::SecurityListener {
    public:
        explicit SecurityListener(Transmitter* transmitter)
            : mTransmitter(transmitter)
        {};

        virtual ~SecurityListener() = default;

        virtual std::string onAcquirePublicKey() override;

        virtual std::vector<uint8_t> onEncryptData(const std::string& pubKey, const std::vector<uint8_t>& src) override;
        virtual std::vector<uint8_t> onDecryptData(const std::vector<uint8_t>& src) override;

        virtual std::string onAcquireDidPropAppId() override;
        virtual std::string onAcquireDidAgentAuthHeader() override;

        virtual std::vector<uint8_t> onSignData(const std::vector<uint8_t>& originData) override;

    private:
        Transmitter* mTransmitter;
    };

    class MessageListener : public elastos::MessageManager::MessageListener {
    public:
        explicit MessageListener(Transmitter* transmitter)
            : mTransmitter(transmitter)
        {};

        virtual ~MessageListener() = default;

        virtual void onStatusChanged(std::shared_ptr<elastos::UserInfo> userInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     elastos::UserInfo::Status status) override;

        virtual void onReceivedMessage(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                       elastos::MessageManager::ChannelType channelType,
                                       const std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo) override;

        virtual void onSentMessage(int msgIndex, int errCode) override;

        virtual void onFriendRequest(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     const std::string& summary) override;

        virtual void onFriendStatusChanged(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                           elastos::MessageManager::ChannelType channelType,
                                           elastos::FriendInfo::Status status) override;

        virtual void onHumanInfoChanged(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                        elastos::MessageManager::ChannelType channelType) override;

    private:
        void HandleStatusChanged(HumanInfo* humanInfo, elastos::HumanInfo::Status status);

    private:
        Transmitter* mTransmitter;
    };

public:
    ~Transmitter()
    {
        s_instance = NULL;
    }

    static std::shared_ptr<Transmitter> Instance(const std::string& path, const std::string& publicKey, IMicroService* service);

    int Start();

    int Stop();

    int SendMessage(const std::string& did, const std::string& msg);

    std::string GetDid();

private:
    Transmitter(IMicroService* service, const std::string& publicKey)
        : mService(service)
        , mPublicKey(publicKey)
    {
    }

    void Init(const std::string& path);

    void HandleMessage(const std::string& humanId, const std::string& type, const std::string& content, int length);

private:
    static Transmitter* s_instance;

    IMicroService* mService;
    std::shared_ptr<Contact> mContact;

    std::string mPublicKey;
};

}

#endif //_ELASTOS_TRANSMITTER_H__
