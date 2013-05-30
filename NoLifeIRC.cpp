#include <SFML/Network.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace std::tr2::sys;
#include "../NoLifeNx/NX.hpp"

namespace NL {
    string Server("embyr.fyrechat.net");
    uint16_t Port(6667);
    string Name("LookupButts");
    sf::TcpSocket Socket;
    istringstream Stream;
    string Source;
    map<string, function<void(void)>> IRCCommands;
    map<string, function<void(string)>> LookupCommands;
    vector<File> Files;
    Node NXBase, NXCharacter, NXEffect, NXEtc, NXItem, NXMap, NXMob, NXMorph, NXNpc, NXQuest, NXReactor, NXSkill, NXSound, NXString, NXTamingMob, NXUI;
    void Send(string s) {
        s += "\r\n";
        Socket.send(s.c_str(), s.length());
    }
    void Message(string target, string msg) {
        size_t p = target.find_first_of('!');
        if (p != target.npos) target.erase(p);
        Send("PRIVMSG " + target + " :" + msg);
    }
    void JoinChannel(string channel) {
        Send("JOIN " + channel);
    }
    void SeekTo(char c) {
        while (Stream.peek() != c) Stream.get();
        Stream.get();
    }
    string GetRest() {
        string msg;
        getline(Stream, msg);
        return msg;
    }
    string GetMessage() {
        SeekTo(':');
        return GetRest();
    }
    string GetNext() {
        string s;
        Stream >> s;
        return s;
    }
    string ToLower(string s) {
        for (char & c : s) c = tolower(c);
        return s;
    }
    void Handle() {
        if (Stream.peek() == ':') {
            Stream.get();
            Stream >> Source;
        }
        function<void(void)> f = IRCCommands[GetNext()];
        if (!f) {
            cout << Stream.str() << endl;
            cout << "Unknown command" << endl;
        } else f();
    }
    void Connect() {
        Socket.setBlocking(true);
        Socket.connect(Server, Port);
        Socket.setBlocking(false);
        Send("NICK " + Name);
        Send("USER " + Name + " 0 * :" + Name);
    }
    void Loop() {
        typedef high_resolution_clock Clock;
        string buf;
        char cbuf[0x1000];
        bool pinged = false;
        Clock::time_point last = Clock::now();
        for (;;) {
            sleep_for(milliseconds(10));
            Clock::time_point now = Clock::now();
            size_t r;
            Socket.receive(cbuf, 0x1000, r);
            if (r) {
                buf.append(cbuf, r);
                for (;;) {
                    size_t p = buf.find_first_of("\r\n");
                    if (p == buf.npos) break;
                    try {
                        Stream = istringstream(buf.substr(0, p));
                        Handle();
                    } catch (...) {
                        cout << "Something did not go right" << endl;
                    }
                    buf.erase(0, p + 2);
                }
                last = now;
                pinged = false;
            } else if (now - last > seconds(10) && !pinged) {
                Send("PING " + Name);
                pinged = true;
            } else if (now - last > seconds(20)) {
                Connect();
                last = now;
                pinged = false;
            }
        }
    }
    void HandleKick() {
        string chan = GetNext();
        Send("JOIN " + chan);
        Message(Source, "I hate you");
    }
    void HandlePing() {
        Send("PONG " + GetMessage());
    }
    void HandleNothing() {}
    void HandlePrivmsg() {
        SeekTo(':');
        if (Stream.get() == '!') {
            string com = ToLower(GetNext());
            if (com == "join") {
                string chan = GetNext();
                JoinChannel(chan);
            } else if (com == "lookup") {
                string type = ToLower(GetNext());
            }
        }
    }
    void LookupMap(string s) {
         
    }
    Node AddFile(char const * s) {
        if (!exists(path(s))) return Node();
        Files.emplace_back(s);
        return Files.back().Base();
    }
    void SetupFiles() {
        if (exists(path("Data.nx"))) {
            NXBase = AddFile("Data.nx");
            NXCharacter = NXBase["Character"];
            NXEffect = NXBase["Effect"];
            NXEtc = NXBase["Etc"];
            NXItem = NXBase["Item"];
            NXMap = NXBase["Map"];
            NXMob = NXBase["Mob"];
            NXMorph = NXBase["Morph"];
            NXNpc = NXBase["Npc"];
            NXQuest = NXBase["Quest"];
            NXReactor = NXBase["Reactor"];
            NXSkill = NXBase["Skill"];
            NXSound = NXBase["Sound"];
            NXString = NXBase["String"];
            NXTamingMob = NXBase["TamingMob"];
            NXUI = NXBase["UI"];
        } else if (exists(path("Base.nx"))) {
            NXBase = AddFile("Base.nx");
            NXCharacter = AddFile("Character.nx");
            NXEffect = AddFile("Effect.nx");
            NXEtc = AddFile("Etc.nx");
            NXItem = AddFile("Item.nx");
            NXMap = AddFile("Map.nx");
            NXMob = AddFile("Mob.nx");
            NXMorph = AddFile("Morph.nx");
            NXNpc = AddFile("Npc.nx");
            NXQuest = AddFile("Quest.nx");
            NXReactor = AddFile("Reactor.nx");
            NXSkill = AddFile("Skill.nx");
            NXSound = AddFile("Sound.nx");
            NXString = AddFile("String.nx");
            NXTamingMob = AddFile("TamingMob.nx");
            NXUI = AddFile("UI.nx");
        } else {
            cout << "OH GOD THE HORROR I CAN'T FIND ANY NX FILES" << endl;
            throw;
        }
    }
    void SetupCommands() {
        IRCCommands["KICK"] = HandleKick;
        IRCCommands["MODE"] = HandleNothing;
        IRCCommands["NOTICE"] = HandleNothing;
        IRCCommands["PING"] = HandlePing;
        IRCCommands["PONG"] = HandleNothing;
        IRCCommands["PRIVMSG"] = HandlePrivmsg;
        IRCCommands["001"] = HandleNothing;
        IRCCommands["002"] = HandleNothing;
        IRCCommands["003"] = HandleNothing;
        IRCCommands["004"] = HandleNothing;
        IRCCommands["005"] = HandleNothing;
        IRCCommands["251"] = HandleNothing;
        IRCCommands["252"] = HandleNothing;
        IRCCommands["254"] = HandleNothing;
        IRCCommands["255"] = HandleNothing;
        IRCCommands["265"] = HandleNothing;
        IRCCommands["266"] = HandleNothing;
        IRCCommands["375"] = HandleNothing;
        IRCCommands["372"] = HandleNothing;
        IRCCommands["376"] = HandleNothing;
        LookupCommands["map"] = LookupMap;
    }
    void IRCBot() {
        SetupFiles();
        SetupCommands();
        Connect();
        Loop();
    }
}

int main() {
    NL::IRCBot();
}