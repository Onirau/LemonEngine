#pragma once

#include <vector>

#include "core/WindowManager.h"
#include "core/LuaBindings.h"
#include "instances/DataModel.h"
#include "instances/services/Lighting.h"
#include "instances/services/Workspace.h"

// Forward declarations
struct lua_State;

// Globals
extern std::vector<BasePart *> g_instances;
extern lua_State *L_main;
extern Camera3D g_camera;