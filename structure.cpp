#pragma once
#include "structure.hpp"
#include <cJSON.h>
#include "log.hpp"
#include "windows.h"
#include <winnt.h>
#include <string>
namespace structure
{
    // structure::structure *structure_ptr;
    std::vector<structure::structure> structures;
    int init(std::string name)
    {
        glog::log("info", "Initializing structures", "structure");
        FILE *file;
        file = fopen(name.c_str(), "rb");
        if (file == NULL)
        {
            return 1;
        }
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *data = (char *)malloc(length + 1);
        fread(data, 1, length, file);
        fclose(file);
        cJSON *json = cJSON_Parse(data);
        free(data);
        if (json == NULL)
        {
            glog::log("error", "Failed to parse JSON file", "structure");
            return 2;
        }
        cJSON *path = cJSON_GetObjectItem(json, "path");
        if (path == NULL)
        {
            glog::log("error", "Failed to get path", "structure");
            return 3;
        }
        std::string path_str = cJSON_GetStringValue(path);
        cJSON *mods = cJSON_GetObjectItem(json, "mods");
        int mods_size = cJSON_GetArraySize(mods);
        for (int i = 0; i < mods_size; i++)
        {
            std::string mod_path = path_str + cJSON_GetStringValue(cJSON_GetObjectItem(cJSON_GetArrayItem(mods, i), "path"));
            FILE *mod_file = fopen(mod_path.c_str(), "rb");
            if (mod_file == NULL)
            {
                glog::log("error", "Failed to open mod file", "structure");
                return 3;
            }
            fseek(mod_file, 0, SEEK_END);
            long mod_length = ftell(mod_file);
            fseek(mod_file, 0, SEEK_SET);
            char *mod_data = (char *)malloc(mod_length + 1);
            fread(mod_data, 1, mod_length, mod_file);
            fclose(mod_file);
            cJSON *mod_json = cJSON_Parse(mod_data);
            free(mod_data);
            if (mod_json == NULL)
            {
                glog::log("error", "Failed to parse mod JSON file", "structure");
                return 4;
            }
            cJSON *dll_path = cJSON_GetObjectItem(mod_json, "path");
            std::string dll_path_str = cJSON_GetStringValue(dll_path);
            dll_path_str = path_str + dll_path_str;
            cJSON *names = cJSON_GetObjectItem(mod_json, "name");
            int names_size = cJSON_GetArraySize(names);
            // std::wstring dll_path_wstr(dll_path_str.begin(), dll_path_str.end());
            HMODULE dll = LoadLibrary(dll_path_str.c_str());
            glog::log("info", "Loaded DLL " + dll_path_str, "structure");
            if (dll == NULL)
            {
                glog::log("error", "Failed to load DLL", "structure");
                glog::log("error", "Error code: " + std::to_string(GetLastError()), "structure");
                return 5;
            }
            for (int j = 0; j < names_size; j++)
            {
                std::string name = cJSON_GetStringValue(cJSON_GetArrayItem(names, j));
                structure::structure structure = (structure::structure)GetProcAddress(dll, name.c_str());
                if (structure == NULL)
                {
                    glog::log("error", "Failed to get fun address " + name, "structure");
                    glog::log("error", "Error code: " + std::to_string(GetLastError()), "structure");
                    continue;
                }
                glog::log("info", "Loaded structure func [" + name + "]", "structure");
                structures.push_back(structure);
            }
        }
    }
}