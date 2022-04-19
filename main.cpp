#include <iostream>

#include "fracturedtime.h"

using namespace std;

namespace Half {

REGISTER_FRACTURED_TIME(int16_t, 2)

}

namespace Hundred {

REGISTER_FRACTURED_TIME(uint32_t, 100)

}

namespace {

template<typename T>
void printTime(const T &tm)
{
    cout << tm.getTh() << " " << tm.getTl() << " = " << tm.timeMS() << " ms" << endl;
}

}

int main()
{
    {
        using namespace Half;
        const auto start = 10_th;
        const auto end = start + 5_th;
        const auto delta = 1_tl;
        auto current = start;

        cout << "~~~~~ HALF TIME ~~~~~" << endl;
        while (current < end) {
            printTime(current);
            current += delta;
        }
    }

    {
        using namespace Hundred;
        cout << "~~~~~ HUNDRED TIME ~~~~~" << endl;
        for (Time i = 5_th; i > 2_th; i-= 33_tl) {
            printTime(i);
        }
    }

    return 0;
}
