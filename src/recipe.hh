#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lilv-0/lilv/lilv.h"

#include "lv2/core/lv2.h"

#ifdef __cplusplus
}
#endif

#include <math.h>
#include <sndfile.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>

#include "bar_helper.hh"
#include "ingredient.hh"
#include "transport.hh"

namespace mouthwash {

// Representation of an auto-generated recipe received from the recipe-generation server
class Recipe {
    public:
    Recipe(LilvWorld* world);
    void AddComponent(std::string name, const LilvPlugin* plugin);

    private:
    BarHelper& barHelper_ = BarHelper::GetInstance();

    LilvWorld*          world_;
    LilvInstance*       instance_;
    std::map<std::string, const LilvPlugin*> plugins_;
    std::map<std::string, Ingredient*> ingredients_;
};

} // mouthwash
