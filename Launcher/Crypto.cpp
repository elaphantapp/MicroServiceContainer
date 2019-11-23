
#include "Crypto.h"
#include "Elastos.Wallet.Utility.h"
#include <sstream>

namespace elastos {

#define _hexc(u) (((u) & 0x0f) + ((((u) & 0x0f) <= 9) ? '0' : 'a' - 0x0a))
#define _hexu(c) (((c) >= '0' && (c) <= '9') ? (c) - '0' : ((c) >= 'a' && (c) <= 'f') ? (c) - ('a' - 0x0a) :\
                  ((c) >= 'A' && (c) <= 'F') ? (c) - ('A' - 0x0a) : -1)

std::shared_ptr<IMicroService::DataHandler> Crypto::sInstance;

std::string Crypto::HexString(uint8_t* data, int len)
{
    std::stringstream ss;
    for (int i = 0; i < len; i++) {
        ss << (uint8_t) _hexc (data[i] >> 4);
        ss << (uint8_t) _hexc (data[i]);
    }

    return ss.str();
}

int Crypto::FromHexString(const std::string& hex, uint8_t* target, int len)
{
    if (0 != hex.size() % 2) {
        return -1;
    }

    const char* source = hex.c_str();
    for (int i = 0; i < len; i++) {
        target[i] = (uint8_t) ((_hexu(source[2 * i]) << 4) | _hexu(source[(2 * i) + 1]));
    }

    return len;
}

std::shared_ptr<IMicroService::DataHandler> Crypto::Instance(const std::string& privateKey)
{
    if (sInstance.get() == nullptr) {
        sInstance = std::shared_ptr<IMicroService::DataHandler>(new Crypto(privateKey));
    }

    return sInstance;
}

std::shared_ptr<std::vector<uint8_t>> Crypto::EncryptData(const std::vector<uint8_t>& data)
{
    return std::make_shared<std::vector<uint8_t>>(data); // ignore encrypt
}

std::shared_ptr<std::vector<uint8_t>> Crypto::DecryptData(const std::vector<uint8_t>& cipherData)
{
    return std::make_shared<std::vector<uint8_t>>(cipherData); // ignore decrypt
}

std::shared_ptr<std::vector<uint8_t>> Crypto::SignData(const std::vector<uint8_t>& data)
{
    uint8_t* signedData;
    int ret = sign(mPrivateKey.c_str(), (void*)data.data(), data.size(), (void**)&signedData);
    if (ret < 0) {
        printf("crypto sign failed\n");
        return std::shared_ptr<std::vector<uint8_t>>();
    }

    return std::make_shared<std::vector<uint8_t>>(signedData, signedData + ret);
}

bool Crypto::VerifyData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signedData)
{
    return false;
}

}
