
#include <getopt.h>
#include <stdio.h>
#include <string>
#include <dlfcn.h>
#include <IMicroService.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "Crypto.h"

using namespace elastos;

typedef IMicroService* (*Creator)(const char* path, const char* pubkey);

typedef void (*Destroy)(IMicroService* service);

std::condition_variable cv;
std::mutex mutex;
bool serviceRunning = false;


void StartService(const std::string& library, const std::string& path, const std::string& privateKey, const std::string& publicKey)
{
    std::shared_ptr<IMicroService::DataHandler> crypto = Crypto::Instance(privateKey);

    void *handle = dlopen(library.c_str(), RTLD_LAZY);
    if (!handle) {
        /* fail to load the library */
        fprintf(stderr, "Error: %s\n", dlerror());
        return;
    }

    Creator CreateService;
    IMicroService* service;
    int ret;

    *(void**)(&CreateService) = dlsym(handle, "CreateService");
    if (CreateService == NULL) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return;
    }

    service = CreateService(path.c_str(), publicKey.c_str());
    if (service == NULL) {
        fprintf(stderr, "Create Service failed!\n");
        dlclose(handle);
        return;
    }

    service->SetDataHandler(crypto);

    ret = service->Start();
    if (ret != 0) {
        fprintf(stderr, "start failed!\n");
        dlclose(handle);
        return;
    }

    std::unique_lock<std::mutex> lk(mutex);
    while (serviceRunning) {
        cv.wait(lk);
    }

    service->Stop();

    Destroy DestroyService;
    *(void**)(&DestroyService) = dlsym(handle, "DestroyService");
    if (CreateService == NULL) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return;
    }

    DestroyService(service);
    dlclose(handle);
}

void StopService(std::thread& workthread) {
    std::unique_lock<std::mutex> lk(mutex);
    serviceRunning = false;
    lk.unlock();
    cv.notify_one();

    workthread.join();
}

int main(int argc, char* argv[])
{
    int c;
    std::string library;
    std::string path;
    std::string privateKey;
    std::string publicKey;

    static struct option long_options[] =
    {
        {"name", required_argument, NULL, 'n'},
        {"path", required_argument, NULL, 'p'},
        {"secret", required_argument, NULL, 's'},
        {"key", required_argument, NULL, 'k'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    while(1) {
        int opt_index = 0;
        c = getopt_long(argc, argv, "n:p:s:k:h", long_options, &opt_index);

        if (-1 == c) {
            break;
        }

        switch(c) {
            case 'n':
                printf("param n: %s\n", argv[optind]);
                library = argv[optind];
                break;
            case 'p':
                printf("param p: %s\n", argv[optind]);
                path = argv[optind];
                break;
            case 's':
                printf("param s: %s\n", argv[optind]);
                privateKey = argv[optind];
                break;
            case 'k':
                printf("param k: %s\n", argv[optind]);
                publicKey = argv[optind];
                break;
            default:
                printf("param default: %s\n", argv[optind]);
                break;
        }
    }

    if (library.empty()) {
        printf("Please set service library by -name!\n");
        return -1;
    }

    if (path.empty()) {
        printf("Please set service local path by -path!\n");
        return -1;
    }

    if (privateKey.empty()) {
        printf("Please set service public key by -secret!\n");
        return -1;
    }

    if (publicKey.empty()) {
        printf("Please set service public key by -key!\n");
        return -1;
    }

    serviceRunning = true;
    std::thread workthread(StartService, library, path, privateKey, publicKey);

    while (1) {
        std::string command;
        std::getline(std::cin, command);
        if (!command.compare("exit")) {
            StopService(workthread);
            break;
        }
    }

    return EXIT_SUCCESS;
}
