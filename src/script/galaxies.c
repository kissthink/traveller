/* 星系运转需要的基础C
 */
#include "core/util.h"
#include "core/zmalloc.h"
#include "net/networking.h"
#include "script/galaxies.h"
#include "script/db.h"
#include "script/net.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

extern NTSnode *g_galaxiesSrvSnode; /* 星系服务端连接 */

extern char g_galaxiesdir[];
extern lua_State *g_srvLuaSt;

int STCallGalaxyFunc(NTSnode *sn) {
    char **argv = &(sn->argv[1]);
    char *funcName = *argv;
    int argc = sn->argc - 1;
    int _m;

    if ('P' != *funcName && 'U' != *funcName && 'B' != *funcName) {
        return GALAXIES_LUA_CALL_ERRNO_FUNC403;
    }

    _m = lua_gettop(g_srvLuaSt);
    if (_m > 0) lua_pop(g_srvLuaSt, _m);

    lua_getglobal(g_srvLuaSt, funcName);
    if (!lua_isfunction(g_srvLuaSt, -1)) {
        TrvLogW("%s not exists", funcName);
        return GALAXIES_LUA_CALL_ERRNO_FUNC404;
    }

    lua_pushinteger(g_srvLuaSt, sn->fd);


    /* 从argv[1]开始，既忽略funcName */
    for (_m = 1; _m < argc; _m++) {
        lua_pushstring(g_srvLuaSt, argv[_m]);
    }


    //argc == count(sn->fd + argv - funcName)
    errno = lua_pcall(g_srvLuaSt, argc, 0, 0);
    if (errno) {
        TrvLogW("%s", lua_tostring(g_srvLuaSt, -1));
        lua_pop(g_srvLuaSt, _m);
        return GALAXIES_LUA_CALL_ERRNO_FUNC502;
    }

    lua_pop(g_srvLuaSt, lua_gettop(g_srvLuaSt));

    return GALAXIES_LUA_CALL_ERRNO_OK;
}

static void STLoginGalaxyGetResult(NTSnode *sn) {
    TrvLogW("%d %s", sn->recv_type, sn->argv[0]);
}

int STLoginGalaxy(char *email, char *password) {
    NTAddReplyMultiString(g_galaxiesSrvSnode, 4, "galaxies", "PUBCitizenLogin", email, password);
    g_galaxiesSrvSnode->proc = STLoginGalaxyGetResult;
    return 0;
}
