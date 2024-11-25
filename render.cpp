#pragma once
#include "render.hpp"
#include "log.hpp"
namespace render
{
    int init(int width, int height)
    {
        glog::log("info", "Initializing Render", "render");
        initgraph(width, height);
        return 0;
    }
    void close()
    {
        glog::log("info", "Closing Render", "render");
        closegraph();
    }
} // namespace render