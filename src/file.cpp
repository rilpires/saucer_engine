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
