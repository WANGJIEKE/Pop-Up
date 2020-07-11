// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include "PopUp.h"

class PopUp {
private:
    std::thread _thread;
    bool _finished;
    bool _gameFailedOnThisPopUp;
    std::function<bool(int)> _shouldFailGame;

public:
    std::wstring text;
    std::wstring caption;
    unsigned int type;

    PopUp(
        const std::wstring& theText,
        const std::wstring& theCaption,
        const unsigned int& theType,
        const std::function<bool(int)>& shouldFailGame = [](int) { return false; }
    ) :
        text(theText),
        caption(theCaption),
        type(theType),
        _thread([this]()
            {
                int ret = MessageBoxRandPos(NULL, text.c_str(), caption.c_str(), type);
                _finished = true;
                _gameFailedOnThisPopUp = _shouldFailGame(ret);
            }
        ),
        _finished(false),
        _gameFailedOnThisPopUp(false),
        _shouldFailGame(shouldFailGame)
    {}

    const std::thread& thread() const { return _thread; }

    const bool& finished() const { return _finished; }

    const bool& gameShouldFail() const { return _gameFailedOnThisPopUp; }
};

void mainLoop(int tickRate)
{

}

int main()
{
    std::thread threads[10];
    for (int i = 0; i < 10; ++i)
    {
        threads[i] = std::thread(
            [i](){
                MessageBoxRandPos(NULL, TEXT("Close Me!"), TEXT("Oops!"), MB_YESNOCANCEL | MB_ICONQUESTION);
            }
        );
    }

    for (int i = 0; i < 10; ++i)
    {
        threads[i].join();
    }
    return 0;
}
