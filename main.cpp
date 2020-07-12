// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "PopUp.h"

struct MessageBoxParam
{
    std::wstring text, caption;
    unsigned int type;
};

static std::vector<MessageBoxParam> msgBoxTemplates
{
    {L"Close Me", L"LOL", MB_OK | MB_ICONINFORMATION},
    {L"You can't close me", L"!", MB_OK | MB_ICONWARNING},
    {L"Hahahaha", L"Haha", MB_OK | MB_ICONINFORMATION},
    {L"_(:3」∠)_", L"Oops!", MB_OK | MB_ICONERROR},
    {L"You are weak. Aren't you?", L"XDDDD", MB_OK | MB_ICONQUESTION }
};

static std::vector<std::future<int>> futures;
static int maxMsgBox;

template<typename T> inline
T myMax(const T& a, const T& b)
{
    return a >= b ? a : b;
}

int MessageBoxWithRandomContent()
{
    static std::random_device rand;
    static std::default_random_engine engine(rand());
    static std::uniform_int_distribution dist(0ULL, msgBoxTemplates.size() - 1);
    const MessageBoxParam& param = msgBoxTemplates[dist(engine)];
    return MessageBoxRandPos(NULL, param.text.c_str(), param.caption.c_str(), param.type);
}

[[noreturn]]
void mainLoop(int spawnIntervalMiliSecond, int initialMsgBox)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    constexpr int tickPerSecond = 25;
    constexpr milliseconds tickLen(1000 / tickPerSecond);
    milliseconds spawnInterval(spawnIntervalMiliSecond);
    milliseconds messageBoxSpawnStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds checkPoint = messageBoxSpawnStart;
    int counter = initialMsgBox;

    while (true)
    {
        milliseconds tickStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

        if (futures.size() == maxMsgBox)
        {
            std::wstringstream ss;
            ss << "The maximum message box number has been reached, GAME OVER." << std::endl;
            ss << "You have close " << myMax(counter - maxMsgBox, 0) << " windows!";
            std::wcout << ss.str() << std::endl;
            HWND hCmd = GetConsoleWindow();
            ShowWindow(hCmd, SW_RESTORE);
            std::cout << "Press enter to exit..." << std::flush;
            std::getchar();
            std::quick_exit(0);
        }

        futures.erase(
            std::remove_if(
                futures.begin(),
                futures.end(),
                [](const std::future<int>& future) { return future.wait_for(0ms) == std::future_status::ready; }
            ),
            futures.end()
        );

        if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - messageBoxSpawnStart >= spawnInterval)
        {
            futures.push_back(
                std::async(std::launch::async, []() { return MessageBoxWithRandomContent(); })
            );
            messageBoxSpawnStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            counter += 1;
        }

        if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - checkPoint >= 500ms && spawnInterval > 0ms)
        {
            spawnInterval -= 10ms;
            checkPoint = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        }

        std::this_thread::sleep_for(
            tickLen - ((duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - tickStart) % tickLen)
        );
    }
}

[[noreturn]]
int main()
{
    int initialMsgBox = 3;
    int spawnInterval = 1000;
    maxMsgBox = 30;

    std::cout << "Close these messagebox as many as possible before they got out of control!" << std::endl;
    std::cout << "Press enter key to start..." << std::flush;
    std::getchar();

    HWND hCmd = GetConsoleWindow();
    ShowWindow(hCmd, SW_MINIMIZE);

    for (int i = 0; i < initialMsgBox; ++i)
    {
        futures.push_back(
            std::async(std::launch::async, MessageBoxWithRandomContent)
        );
    }
    mainLoop(spawnInterval, initialMsgBox);
}
