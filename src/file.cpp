#include "file.h"
#include "core.h"

YAML::Node  YAML::convert<Vector2>::encode(const Vector2& v) {
    YAML::Node node;
    node.SetStyle(YAML::EmitterStyle::Flow);
    node.push_back(v.x);
    node.push_back(v.y);
    return node;
}
bool        YAML::convert<Vector2>::decode(const YAML::Node& node, Vector2& v) {
    if(!node.IsSequence() || node.size() != 2) { return false; }
    v.x = node[0].as<float>();
    v.y = node[1].as<float>();
    return true;
}

YAML::Node  YAML::convert<Color>::encode(const Color& c) {
    YAML::Node node;
    node.SetStyle(YAML::EmitterStyle::Flow);
    node.push_back<int>(c.r);
    node.push_back<int>(c.g);
    node.push_back<int>(c.b);
    node.push_back<int>(c.a);
    return node;
}
bool        YAML::convert<Color>::decode(const YAML::Node& node, Color& c) {
    if(!node.IsSequence() || node.size() != 4) { return false; }
    c.r = node[0].as<int>();
    c.g = node[1].as<int>();
    c.b = node[2].as<int>();
    c.a = node[3].as<int>();
    return true;
}

YAML::Node  YAML::convert<Rect>::encode(const Rect& r) {
    YAML::Node node;
    node.SetStyle(YAML::EmitterStyle::Flow);
    node.push_back(r.top_left.x);
    node.push_back(r.top_left.y);
    node.push_back(r.bottom_right.x);
    node.push_back(r.bottom_right.y);
    return node;
}
bool        YAML::convert<Rect>::decode(const YAML::Node& node, Rect& r) {
    if(!node.IsSequence() || node.size() != 4) { return false; }
    r.top_left.x = node[0].as<float>();
    r.top_left.y = node[1].as<float>();
    r.bottom_right.x = node[2].as<float>();
    r.bottom_right.y = node[3].as<float>();
    return true;
}
