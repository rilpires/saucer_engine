#ifndef SAUCER_FILE_H
#define SAUCER_FILE_H

#include <yaml-cpp/yaml.h>

#include "vector.h"
#include "rect.h"
#include "color.h"
#include "resources.h"

#define DECLARE_YAML_CONVERT(T)                     \
template<>                                          \
struct YAML::convert<T>{                            \
    static YAML::Node encode(const T& v);           \
    static bool decode(const YAML::Node& , T& );    \
};

DECLARE_YAML_CONVERT(Vector2);
DECLARE_YAML_CONVERT(Color);
DECLARE_YAML_CONVERT(Rect);


#endif