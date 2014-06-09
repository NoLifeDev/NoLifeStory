//////////////////////////////////////////////////////////////////////////////
// NoLifeNxBench - Part of the NoLifeStory project                          //
// Copyright © 2014 Peter Atashian                                          //
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

#include <nx/node.hpp>
#include <nx/nx.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <regex>


namespace nl {
    using string = std::string;
    template <typename T>
    using vector = std::vector<T>;
    struct timemob {
        int mapid;
        int mobid;
        int start_hour;
        int end_hour;
        string message;
    };
    vector<timemob> timemob_data;
    std::regex make_reg(std::string const & s) {
        return std::regex(s, std::regex_constants::extended | std::regex_constants::icase | std::regex_constants::optimize);
    }
    namespace reg {
        auto endhour = make_reg("endhour");
        auto id = make_reg("id");
        auto info = make_reg("info");
        auto mapx = make_reg("Map[0-9]");
        auto message = make_reg("message");
        auto starthour = make_reg("starthour");
        auto timemob = make_reg("timemob");
    }
    bool match(node const & n, std::regex const & r) {
        return std::regex_match(n.name(), r);
    }
    class mcdb {
    public:
        mcdb() {
            nx::load_all();
        }
        void dump() {
            for (auto n1 : nx::map) {
                if (match(n1, reg::mapx)) {
                    for (auto n2 : n1) {
                        for (auto n3 : n2) {
                            if (match(n3, reg::info)) {
                                for (auto n4 : n3) {
                                    if (match(n4, reg::timemob)) {
                                        timemob d;
                                        d.mapid = 0;
                                        for (auto n5 : n4) {
                                            if (match(n5, reg::endhour)) d.end_hour = n5;
                                            else if (match(n5, reg::endhour)) d.end_hour = n5;
                                            else if (match(n5, reg::endhour)) d.end_hour = n5;
                                            else if (match(n5, reg::endhour)) d.end_hour = n5;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }
    private:
    };
}
int main() {
    nl::mcdb().dump();
}
