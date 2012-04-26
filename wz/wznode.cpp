#include "wz.h"
#include "wzmain.h"
#include "wzimg.h"

namespace WZ {
    class Node::Data {
    public:
        Data(const Node& parent, string name)
            :intValue(0), floatValue(0), parent(parent), name(name) {}
        string stringValue;
        double floatValue;
        int intValue;
        //Sprite sprite;
        //Sound sound;
        Node parent;
        string name;
        map <string, Node> children;
    private:
        Data()
            :intValue(0), floatValue(0) {}
        Data(const Node::Data&);
    };

    Node::Node() : data(nullptr) {}

    Node::Node(const Node& other) : data(other.data) {}

    Node::Node(const Node& other, string name) : data(new Data(other, name)) {}

    Node& Node::operator= (const Node& other) {
        data = other.data;
        return *this;
    }

    Node Node::operator[] (const string key) const {
        if (!data) return Node();
        if (key == "..") return data->parent;
        if (key == ".") return *this;
        auto n = data->children.find(key);
        if (n == data->children.end()) return Node();
        return n->second;
    }

    Node Node::operator[] (const char key[]) const {
        return (*this)[(string)key];
    }

    Node Node::operator[] (const int key) const {
        return (*this)[to_string(key)];
    }
    Node Node::operator[] (const Node key) const {
        return (*this)[(string)key];
    }

    Node Node::g(const string key) {
        return data->children.emplace(key, Node(*this, key)).first->second;
    }

    map<string, Node>::const_iterator Node::begin() const {
        if (!data) return map<string, Node>::const_iterator();
        return data->children.begin();
    }

    map<string, Node>::const_iterator Node::end() const {
        if (!data) return map<string, Node>::const_iterator();
        return data->children.end();
    }

    map<string, Node>::const_reverse_iterator Node::rbegin() const {
        if (!data) return map<string, Node>::const_reverse_iterator();
        return data->children.rbegin();
    }

    map<string, Node>::const_reverse_iterator Node::rend() const {
        if (!data) return map<string, Node>::const_reverse_iterator();
        return data->children.rend();
    }

    string Node::Name() const {
        if (!data) return string();
        return data->name;
    }

    void Node::InitTop(const string s) {
        data = new Data(Node(), s);
    }

    void Node::Assign(const Node other) {
        data = other.data;
    }

    Node::operator bool() const {
        return (bool)data;
    }

    Node::operator string() const {
        if (!data) return string();
        return data->stringValue;
    }

    Node::operator double() const {
        if (!data) return 0;
        return data->floatValue;
    }

    Node::operator int() const {
        if (!data) return 0;
        return data->intValue;
    }

    /*Node::operator Sprite() const {
        if (!data) return Sprite();
        return data->sprite;
    }

    Node::operator Sound() const {
        if (!data) return Sound();
        return data->sound;
    }*/

    void Node::Set(const string v) {
        //data->intValue = stoi(v);
        //data->floatValue = stod(v);
        data->intValue = 0;
        data->floatValue = 0;
        data->stringValue = v;
    }

    void Node::Set(const double v) {
        data->intValue = (int)v;
        data->floatValue = v;
        data->stringValue = to_string(v);
    }

    void Node::Set(const int v) {
        data->intValue = v;
        data->floatValue = v;
        data->stringValue = to_string(v);
    }

    /*void Node::Set(const Sprite& v) {
        data->sprite = v;
    }

    void Node::Set(const Sound& v) {
        data->sound = v;
    }*/

    void Node::Recurse() {
        if (!data) return;
        string s = data->children["UOL"];
        if (!s.empty()) {
	        vector<string> parts;
            string str;
	        for (char c : s) {
		        if (c == '/') {
                    parts.push_back(str);
                    str.clear();
                } else str += c;
	        }
            parts.push_back(str);
            Node nn = data->parent;
            for (string str : parts) {
                if (!nn) break;
                nn = nn[str];
            }
            if (nn) nn.data = data;
        } else {
            for (pair<string, Node> n : data->children) {
                n.second.Recurse();
            }
        }
    }
}