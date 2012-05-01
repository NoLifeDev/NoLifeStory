///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "../wz/wz.h"
#include <iostream>
#include <thread>
#include <set>
using namespace std;
using namespace chrono;
using namespace WZ;

int main() {
    high_resolution_clock clock;
    AddPath("");
    //AddPath("C:/Nexon/MapleStory/");
    AddPath("C:/Nexon/MapleBeta 40/");
    AddPath("C:/Nexon/MapleStory 109/");
    AddPath("D:/Program Files (x86)/MapleStory_gms_109/");
    high_resolution_clock::time_point start = clock.now();
    Init(true);
    high_resolution_clock::time_point end = clock.now();
    Node n = Base["Map"]["Map"];
	set<string> alreadyFound;
	for (int i = 0; i < 10; ++i) {
		Node nn = n[string("Map")+to_string(i)];
		for (Node mapdata : nn) {
			Node info = mapdata["info"];
			for (Node infoNode : info) {
				string name = infoNode.Name();
				if (alreadyFound.find(name) == alreadyFound.end()) {
					alreadyFound.insert(name);
					cout << "Found new Info Type: " << name << endl;
				}
			}
		}
	}
    cout << "Time taken: " << duration_cast<milliseconds>(end-start).count() << " ms" << endl;
    
}