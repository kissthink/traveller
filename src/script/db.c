#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "sqlite3.h"

#include "core/sds.h"
#include "core/adlist.h"
#include "core/dict.h"
#include "core/util.h"
#include "core/zmalloc.h"
#include "core/ini.h"
#include "core/extern.h"

#include "script/script.h"

// for lua
// 连接db
// @param filepath string
int STDB_Connect(lua_State *L) {
    int errno;
    sqlite3 *db;

    const char *filepath = lua_tostring(L, 1);

    errno = sqlite3_open(filepath, &db);
    if (0 == errno) {
        lua_pushlightuserdata(L, db);
    } else {
        lua_pushnil(L);
        sqlite3_close(db);
    }

    return 1;
}

// for lua
// 关闭db连接
// @param db *db
int STDB_Close(lua_State *L) {
    int errno;
    sqlite3 *db;
    db = lua_touserdata(L, 1);
    C_Assert((0 != db), "STDB_Close error");

    sqlite3_close(db);

    return 1;
}


// for lua
// 执行sql语句
// @param db  *db
// @param sql string
int STDB_Query(lua_State *L) {
    sqlite3 *db = lua_touserdata(L, 1);
    C_Assert((0 != db), "STDB_Close error");

    const char *sql = lua_tostring(L, 2);
    char **dbresult;
    char *errmsg;
    int errno, nrow, ncolumn, loopI, loopJ, index;

    if (0 == sql) return 0;

    errno = sqlite3_get_table(
            db,
            sql,
            &dbresult,
            &nrow,
            &ncolumn,
            &errmsg);

    if (SQLITE_OK != errno) {
        C_UtilLogW("STDB_Query Error: %s", errmsg);
        sqlite3_free(errmsg);

        lua_pushnil(L);
        return 1;
    }


    lua_newtable(L);

    index = ncolumn;
    for (loopI = 0; loopI < nrow; loopI++) {
        lua_pushnumber(L, loopI);

        lua_newtable(L);

        for (loopJ = 0; loopJ < ncolumn; loopJ++) {
            lua_pushstring(L, dbresult[loopJ]);
            lua_pushstring(L, dbresult[index]);
            lua_settable(L, -3);

            index++;
        }

        lua_settable(L, -3);
    }

    sqlite3_free_table(dbresult);

    return 1;
}
