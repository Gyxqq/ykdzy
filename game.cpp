#pragma once
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"

int game::init(std::string name)
{
    glog::log("info", "Initializing Game: " + name, "game");
    this->world.load(name);
    return 0;
}

int game::save()
{
    glog::log("info", "Saving Game", "game");
    this->world.save();
    return 0;
}