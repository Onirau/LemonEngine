#include "Workspace.h"
#include "../../core/LuaClassBinder.h"
#include "../BasePart.h"
#include "../DataModel.h"
#include "../Part.h"
#include <cmath>

Workspace::Workspace() : Instance("Workspace") { Name = "Workspace"; }

void Workspace::Bind(lua_State *L) {
    LuaClassBinder::RegisterClass("Workspace", "Instance");

    // Gravity property
    LuaClassBinder::AddProperty(
        "Workspace", "Gravity",
        [](lua_State *L, Instance *inst) -> int {
            auto *ws = static_cast<Workspace *>(inst);
            Vector3 *v =
                (Vector3 *)lua_newuserdata(L, sizeof(Vector3));
            *v = ws->Gravity;
            luaL_getmetatable(L, "Vector3Meta");
            lua_setmetatable(L, -2);
            return 1;
        },
        [](lua_State *L, Instance *inst, int valueIdx) -> int {
            auto *ws = static_cast<Workspace *>(inst);
            Vector3 *v =
                (Vector3 *)luaL_checkudata(L, valueIdx, "Vector3Meta");
            ws->Gravity = *v;
            return 0;
        });

    // CurrentCamera property
    LuaClassBinder::AddProperty(
        "Workspace", "CurrentCamera",
        [](lua_State *L, Instance *inst) -> int {
            auto *ws = static_cast<Workspace *>(inst);
            if (ws->CurrentCamera) {
                // Push camera as light userdata or custom type
                lua_pushlightuserdata(L, ws->CurrentCamera);
            } else {
                lua_pushnil(L);
            }
            return 1;
        },
        nullptr); // Read-only for now
}