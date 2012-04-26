#include "../wz/wz.h"

int main() {
    clock_t start = clock();
    WZ::Paths.push_back("");
	WZ::Paths.push_back("C:/Nexon/MapleStory/");
    WZ::Init();
    clock_t end = clock();
    cout << end - start << endl;
    cin.get();
}