var luanode = null;

try{
    luanode = require('../build/Release/luanode');
}catch(e){
    luanode = require('../build/Debug/luanode');
}

module.exports = luanode;
