//////////////////////////////////////////////////////////////////////////////
// NoLifeIRC - Part of the NoLifeStory project                              //
// Copyright © 2013 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include <nx/nx.hpp>
#include <nx/node.hpp>
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

namespace nl {
namespace irc {
std::string server_url = "irc.fyrechat.net";
uint16_t server_port = {6667};
std::string username = "LookupButts";
sf::TcpSocket socket;
std::istringstream stream;
std::string source;
std::map<std::string, std::function<void(void)>> commands_irc;
std::map<std::string, std::function<void(std::string)>> commands_lookup;
void send_raw(std::string s) {
    if (s.size() > 510) s.resize(510);
    s += "\r\n";
    socket.send(s.c_str(), s.length());
}
void send_reply(std::string msg) {
    auto target = source;
    auto p = target.find_first_of('!');
    if (p != std::string::npos) target.erase(p);
    send_raw("PRIVMSG " + target + " :" + msg);
}
void join_channel(std::string channel) { send_raw("JOIN " + channel); }
void seek_to(char c) {
    while (stream.peek() != c) stream.get();
    stream.get();
}
std::string get_rest() {
    while (stream.peek() == ' ') stream.get();
    std::string msg;
    std::getline(stream, msg);
    return msg;
}
std::string get_message() {
    seek_to(':');
    return get_rest();
}
std::string get_next() {
    std::string s;
    stream >> s;
    return s;
}
std::string to_lower(std::string s) {
    for (char & c : s) c = std::tolower(c, std::locale::classic());
    return s;
}
void handle() {
    std::cout << stream.str() << std::endl;
    if (stream.peek() == ':') {
        stream.get();
        stream >> source;
    }
    auto f = commands_irc[get_next()];
    if (f) f();
}
void connect() {
    socket.setBlocking(true);
    socket.disconnect();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    socket.connect(server_url, server_port);
    socket.setBlocking(false);
    send_raw("NICK " + username);
    send_raw("USER " + username + " 0 * :" + username);
    std::cout << "Joined server" << std::endl;
}
void loop() {
    std::string buf;
    char cbuf[0x10000];
    bool pinged = false;
    auto last = std::chrono::high_resolution_clock::now();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto now = std::chrono::high_resolution_clock::now();
        size_t r;
        socket.receive(cbuf, 0x10000, r);
        if (r) {
            buf.append(cbuf, r);
            for (;;) {
                size_t p = buf.find_first_of("\r\n");
                if (p == std::string::npos) break;
                try {
                    stream = std::istringstream(buf.substr(0, p));
                    handle();
                } catch (std::exception e) {
                    std::cout << "Something did not go right" << std::endl;
                }
                buf.erase(0, p + 2);
            }
            last = now;
            pinged = false;
        } else if (now - last > std::chrono::seconds(10) && !pinged) {
            send_raw("PING " + username);
            pinged = true;
        } else if (now - last > std::chrono::seconds(20)) {
            std::cout << "Timed out" << std::endl;
            connect();
            last = now;
            pinged = false;
        }
    }
}
void handle_kick() {
    auto chan = get_next();
    send_raw("JOIN " + chan);
    send_reply("I hate you");
}
void handle_ping() { send_raw("PONG " + get_message()); }
void handle_nothing() {}
void handle_privmsg() {
    seek_to(':');
    if (stream.get() == '!') {
        auto com = to_lower(get_next());
        if (com == "join") {
            auto chan = get_next();
            join_channel(chan);
        } else if (com == "lookup") {
            auto type = to_lower(get_next());
            auto f = commands_lookup[type];
            if (f) f(get_rest());
        } else if (com == "say") {}
    }
}
void lookup_map(std::string s) {
    send_reply("Looking up map with Name/ID of " + s);
    auto found_anything = false;
    auto number = 0;
    for (auto n1 : nx::string["Map.img"])
        for (auto n2 : n1) {
            auto found = n2.name().find(s) != std::string::npos
                         || std::any_of(n2.begin(), n2.end(), [&](node n) {
                             return n.get_string().find(s) != std::string::npos;
                         });
            if (!found) continue;
            found_anything = true;
            std::string result = "map " + n2.name() + " { ";
            for (auto n3 : n2) result += n3.name() + " = \"" + n3.get_string() + "\"; ";
            result += "};";
            send_reply(result);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (++number > 10) {
                send_reply("Too many results found. Aborting.");
                return;
            }
        }
    if (found_anything)
        send_reply("End of results");
    else
        send_reply("No results found");
}
void setup_commands() {
    commands_irc["KICK"] = &handle_kick;
    commands_irc["PING"] = &handle_ping;
    commands_irc["PRIVMSG"] = &handle_privmsg;
    commands_lookup["map"] = &lookup_map;
}
void irc() {
    nx::load_all();
    setup_commands();
    connect();
    loop();
}
};
}

int main() { nl::irc::irc(); }
