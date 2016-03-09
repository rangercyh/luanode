# luanode
lua binding for node

the code based on Brett's NodeLua: https://github.com/brettlangdon/NodeLua, thanks for him! He is a very kind man!

this code is used for node version upper about 0.12.x

so just use it!

if you find any question, welcome to submit pullrequest :)

## Install

Lua and it's C libraries are required for this module to work.

```bash
npm install luanode
```

```nodejs
var Luanode = require('luanode');
var lua = new Luanode.LuaState('luanode');
lua.setGlobal('abc', 1);    -- set global var abc to 1
lua.setGlobal('key', 123)
print(lua.getGlobal('key')) --get global var key
lua.close()             -- shutdown the lua state return 0 or errcode or yield
print(lua.getName())    -- get lua state name luanode
lua.push(123)           -- push 123 to the lua stack
print(lua.pop())        -- get num values on the stack. Default is 1.
lua.push(1)
lua.push(2)
lua.push(3)
print(lua.pop(3))
lua.getTop()            -- return the stack's topvalue
lua.setTop(10)          -- set the lua stack's top to index
lua.replace(-2)         -- replace the stack's topvalue to the specified index
lua.status()            -- return the current status code for lua. 0 or errcode or yield
lua.collectGarbage(Luanode.GC.STOP) -- control the lua garbage collector
lua.doFile('test.lua')          -- load and excute lua code stored in file
lua.doString('local a = 1; print(a);')      -- excute lua string
var s = "print(123)"
lua.doString(s)

-- some default lua value
print(Luanode.INFO.VERSION)
print(Luanode.INFO.VERSION_NUM)
print(Luanode.INFO.COPYRIGHT)
print(Luanode.INFO.AUTHORS)

print(Luanode.STATUS.YIELD)
print(Luanode.STATUS.ERRRUN)
print(Luanode.STATUS.ERRSYNTAX)
print(Luanode.STATUS.ERRMEM)
print(Luanode.STATUS.ERRERR)

print(Luanode.GC.STOP)
print(Luanode.GC.RESTART)
print(Luanode.GC.COLLECT)
print(Luanode.GC.COUNT)
print(Luanode.GC.COUNTB)
print(Luanode.GC.STEP)
print(Luanode.GC.SETPAUSE)
print(Luanode.GC.SETSTEPMUL)
```

Licence
=====

[WTFPL](http://en.wikipedia.org/wiki/WTFPL) 
