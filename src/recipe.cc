#include "recipe.hh"

using namespace mouthwash;

Recipe::Recipe(LilvWorld* world)
    : world_(world)
{
}

void
Recipe::AddComponent(std::string name, const LilvPlugin* plugin)
{
    ingredients_[name] = new Ingredient(world_, plugin);

    //TODO setup yoshimi parameters
    //TODO send midi file to yoshimi
    //TODO collect audio output from yoshimi
    
    //TODO setup mapping of soundfiles to input/output ports for soundfiles with
    //multiple channels
}
