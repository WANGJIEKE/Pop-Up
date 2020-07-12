// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "PopUp.h"

struct MessageBoxParam
{
    std::wstring text, caption;
    unsigned int type;
};

static std::vector<MessageBoxParam> msgBoxParams
{
    {L"Close Me", L"LOL", MB_OK | MB_ICONINFORMATION},
    {L"You can't close me", L"!", MB_OK | MB_ICONINFORMATION},
    {L"Hahahaha", L"Haha", MB_OK | MB_ICONINFORMATION},
    {L"_(:3」∠)_", L"Oops!", MB_OK | MB_ICONERROR}
};

static std::vector<std::future<int>> futures;
static int maxMsgBox;

int MessageBoxWithRandomContent()
{
    std::random_device rand;
    std::default_random_engine engine(rand());
    std::uniform_int_distribution dist(0ULL, msgBoxParams.size() - 1);
    const MessageBoxParam& param = msgBoxParams[dist(engine)];
    return MessageBoxRandPos(NULL, param.text.c_str(), param.caption.c_str(), param.type);
}

void mainLoop(int spawnIntervalMiliSecond)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    constexpr int tickPerSecond = 25;
    constexpr milliseconds tickLen(1000 / tickPerSecond);
    milliseconds spawnInterval(spawnIntervalMiliSecond);
    milliseconds messageBoxSpawnStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    while (true)
    {
        milliseconds tickStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

        if (futures.size() == maxMsgBox)
        {
            std::cout << "You lose because the maximum message box number has been reached" << std::endl;
            return;
        }

        futures.erase(
            std::remove_if(
                futures.begin(),
                futures.end(),
                [](const std::future<int>& future) { return future.wait_for(0ms) == std::future_status::ready; }
            ),
            futures.end()
        );
        if (futures.empty())
        {
            std::cout << "You win because you just close all message boxes before a new one is generated" << std::endl;
            return;
        }

        if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - messageBoxSpawnStart >= spawnInterval)
        {
            futures.push_back(
                std::async(std::launch::async, []() { return MessageBoxWithRandomContent(); })
            );
            messageBoxSpawnStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        }

        std::this_thread::sleep_for(
            milliseconds(
                tickLen - ((duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - tickStart) % tickLen)
            )
        );
    }
}

int main()
{
    int initialMsgBox = 3;
    int spawnInterval = 1500;
    maxMsgBox = 20;

    for (int i = 0; i < initialMsgBox; ++i)
    {
        futures.push_back(
            std::async(std::launch::async, MessageBoxWithRandomContent)
        );
    }
    mainLoop(spawnInterval);
    return 0;
}
