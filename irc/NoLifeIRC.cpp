#include <SFML/Network.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
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
    void Send(string s) {
        if (s.size() > 510) s.resize(510);
        s += "\r\n";
        Socket.send(s.c_str(), s.length());
    }
    void Message(string msg) {
        string target(Source);
        size_t p(target.find_first_of('!'));
        if (p != string::npos) target.erase(p);
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
        while (Stream.peek() == ' ') Stream.get();
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
        if (f) f();
    }
    void Connect() {
        Socket.setBlocking(true);
        Socket.disconnect();
        Socket.connect(Server, Port);
        Socket.setBlocking(false);
        Send("NICK " + Name);
        Send("USER " + Name + " 0 * :" + Name);
        cout << "Joined server" << endl;
    }
    void Loop() {
        typedef high_resolution_clock Clock;
        string buf;
        char cbuf[0x10000];
        bool pinged = false;
        Clock::time_point last = Clock::now();
        for (;;) {
            sleep_for(milliseconds {100});
            Clock::time_point now = Clock::now();
            size_t r;
            Socket.receive(cbuf, 0x10000, r);
            if (r) {
                buf.append(cbuf, r);
                for (;;) {
                    size_t p = buf.find_first_of("\r\n");
                    if (p == string::npos) break;
                    try {
                        Stream = istringstream(buf.substr(0, p));
                        Handle();
                    } catch (std::exception e) {
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
                cout << "Timed out" << endl;
                Connect();
                last = now;
                pinged = false;
            }
        }
    }
    void HandleKick() {
        string chan = GetNext();
        Send("JOIN " + chan);
        Message("I hate you");
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
                function<void(string)> f = LookupCommands[type];
                if (f) f(GetRest());
            } else if (com == "say") {

            }
        }
    }
    void LookupMap(string s) {
        Message("Looking up map with Name/ID of " + s);
        bool found = false;
        for (Node n1 : NXString["Map.img"]) for (Node n2 : n1) {
            if (n2.Name().find(s) != string::npos ||
                string(n2["mapDesc"]).find(s) != string::npos ||
                string(n2["mapName"]).find(s) != string::npos ||
                string(n2["streetName"]).find(s) != string::npos) {
                    found = true;
                    string result("ID {" + n2.Name() + "}");
                    if (n2["mapName"]) result += " Name {" + n2["mapName"] + "}";
                    if (n2["streetName"]) result += " Area {" + n2["streetName"] + "}";
                    if (n2["mapDesc"]) result += " Description {" + n2["mapDesc"] + "}";
                    Message(result);
                    sleep_for(milliseconds(500));
            }
        }
        if (found) Message("End of results");
        else Message("No results found");
    }
    void SetupCommands() {
        IRCCommands["KICK"] = HandleKick;
        IRCCommands["PING"] = HandlePing;
        IRCCommands["PRIVMSG"] = HandlePrivmsg;
        LookupCommands["map"] = LookupMap;
    }
    void IRCBot() {
        LoadAllNX();
        SetupCommands();
        Connect();
        Loop();
    }
}

int main() {
    NL::IRCBot();
}