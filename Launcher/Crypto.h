
#ifndef __ELASTOS_CRYPTO_H__
#define __ELASTOS_CRYPTO_H__

#include <memory>
#include <string>
#include "IMicroService.h"

namespace elastos {

class Crypto : public IMicroService::DataHandler {

public:
    static std::shared_ptr<IMicroService::DataHandler> Instance(const std::string& privateKey);

    std::shared_ptr<std::vector<uint8_t>> EncryptData(const std::vector<uint8_t>& data);

    std::shared_ptr<std::vector<uint8_t>> DecryptData(const std::vector<uint8_t>& cipherData);

    std::shared_ptr<std::vector<uint8_t>> SignData(const std::vector<uint8_t>& data);

    bool VerifyData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signedData);

private:
    Crypto(const std::string& privateKey)
        : mPrivateKey(privateKey)
    {}

    std::string HexString(uint8_t* data, int len);

    int FromHexString(const std::string& hex, uint8_t* target, int len);

private:
    static std::shared_ptr<IMicroService::DataHandler> sInstance;

    std::string mPrivateKey;

};

}


#endif // __ELASTOS_CRYPTO_H__
