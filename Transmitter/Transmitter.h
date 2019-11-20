
#ifndef _ELASTOS_TRANSMITTER_H__
#define _ELASTOS_TRANSMITTER_H__

#include <memory>
#include <string>
#include "IMicroService.h"
#include "Contact.hpp"

namespace elastos {

class Transmitter {
private:
    class Listener : public ElaphantContact::Listener {
    public:
        explicit Listener(Transmitter* transmitter)
            : mTransmitter(transmitter)
        {};

        virtual ~Listener() = default;

        virtual std::shared_ptr<std::vector<uint8_t>> onAcquire(const AcquireArgs& request) override;

        virtual void onEvent(EventArgs& event) override;

        virtual void onReceivedMessage(const std::string& humanCode,
                                       ContactChannel channelType,
                                       std::shared_ptr<ElaphantContact::Message> msgInfo) override;

        virtual void onError(int errCode, const std::string& errStr,
                             const std::string& ext) override;
    private:
        void HandleStatusChanged(const std::string& humanCode, elastos::HumanInfo::Status status);

    private:
        Transmitter* mTransmitter;
    };

    class DataListener : public ElaphantContact::DataListener {
    public:
        explicit DataListener(Transmitter* transmitter)
            : mTransmitter(transmitter)
        {};

        virtual ~DataListener() = default;

        virtual void onNotify(const std::string& humanCode,
                              ContactChannel channelType,
                              const std::string& dataId, int status) override;

        virtual int onReadData(const std::string& humanCode,
                               ContactChannel channelType,
                               const std::string& dataId, uint64_t offset,
                               std::vector<uint8_t>& data) override;

        virtual int onWriteData(const std::string& humanCode,
                                ContactChannel channelType,
                                const std::string& dataId, uint64_t offset,
                                const std::vector<uint8_t>& data) override;
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

    int GetFriendList(std::stringstream* info);

private:
    Transmitter(IMicroService* service, const std::string& publicKey)
        : mService(service)
        , mPublicKey(publicKey)
    {
    }

    void Init(const std::string& path);

    void HandleMessage(const std::string& humanId, const std::string& type, const std::string& content, int length);

    std::shared_ptr<std::vector<uint8_t>> HandleData(int type, const std::vector<uint8_t>& data);

    std::string GetAgentAuthHeader();

private:
    static std::shared_ptr<Transmitter> s_instance;

    IMicroService* mService;
    std::shared_ptr<ElaphantContact> mContact;
    std::shared_ptr<Listener> mListener;
    std::shared_ptr<DataListener> mDataListener;

    std::string mPublicKey;
};

}

#endif //_ELASTOS_TRANSMITTER_H__
