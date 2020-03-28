#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lilv-0/lilv/lilv.h"

#include "lv2/core/lv2.h"

#ifdef __cplusplus
}
#endif

#include <memory>
#include <math.h>
#include <iostream>
#include <map>
#include <vector>
#include <sndfile.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bar_helper.hh"
#include "ingredient.hh"
#include "recipe.hh"

namespace mouthwash {

// Singleton class describing Lilv and LV2 components necessary to manipulate
// ingredients
class Bar {
    public:
    static Bar& GetInstance()
    {
        static Bar instance;
        return instance;
    }

    private:
    Bar();
    ~Bar();
    void SetupRecipe();

    LilvWorld*          world_;
    std::map<std::string, const LilvPlugin*> plugins_;
    // map connections to recipes
    std::map<std::string, Recipe*> recipes_;

    BarHelper& barHelper_ = BarHelper::GetInstance();

    public:
    Bar(Bar const&)             = delete;
    void operator=(Bar const&)    = delete;
};

} // mouthwash
