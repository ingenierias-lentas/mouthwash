#include "bar.hh"

#include <cstdlib>

using namespace mouthwash;

Bar::Bar()
{
    putenv((char*)"LV2_PATH=/l/l/lib/lv2");
    world_ = lilv_world_new();
    lilv_world_load_all(world_);
    const LilvPlugins* plugins = lilv_world_get_all_plugins(world_);
    LILV_FOREACH(plugins, i, plugins) {
        //TODO should really get md5sum of plugin directory
        const LilvPlugin* plugin = lilv_plugins_get(plugins, i);
        std::string pluginUri = std::string(lilv_node_as_uri(lilv_plugin_get_uri(plugin)));
        plugins_[pluginUri] = plugin;
    }
}

Bar::~Bar()
{
    lilv_world_free(world_);
}


void
Bar::SetupRecipe()
{
    recipes_["default"] = new Recipe(world_);
    recipes_["default"]->AddComponent(
        "yoshimi-default",
        plugins_.at("http://yoshimi.sourceforge.net/lv2_plugin")
    );
}
