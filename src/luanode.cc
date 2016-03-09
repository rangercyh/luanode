#include <node.h>
#include <v8.h>

#include "luastate.h"

extern "C"{
#include <lua.h>
}

using namespace v8;

void init_info_constants(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Object> constants = Object::New(isolate);
    constants->Set(String::NewFromUtf8(isolate, "VERSION"), String::NewFromUtf8(isolate, LUA_VERSION));
    constants->Set(String::NewFromUtf8(isolate, "VERSION_NUM"), Number::New(isolate, LUA_VERSION_NUM));
    constants->Set(String::NewFromUtf8(isolate, "COPYRIGHT"), String::NewFromUtf8(isolate, LUA_COPYRIGHT));
    constants->Set(String::NewFromUtf8(isolate, "AUTHORS"), String::NewFromUtf8(isolate, LUA_AUTHORS));
    exports->Set(String::NewFromUtf8(isolate, "INFO"), constants);
}


void init_status_constants(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Object> constants = Object::New(isolate);
    constants->Set(String::NewFromUtf8(isolate, "YIELD"), Number::New(isolate, LUA_YIELD));
    constants->Set(String::NewFromUtf8(isolate, "ERRRUN"), Number::New(isolate, LUA_ERRRUN));
    constants->Set(String::NewFromUtf8(isolate, "ERRSYNTAX"), Number::New(isolate, LUA_ERRSYNTAX));
    constants->Set(String::NewFromUtf8(isolate, "ERRMEM"), Number::New(isolate, LUA_ERRMEM));
    constants->Set(String::NewFromUtf8(isolate, "ERRERR"), Number::New(isolate, LUA_ERRERR));
    exports->Set(String::NewFromUtf8(isolate, "STATUS"), constants);
}


void init_gc_constants(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Object> constants = Object::New(isolate);
    constants->Set(String::NewFromUtf8(isolate, "STOP"), Number::New(isolate, LUA_GCSTOP));
    constants->Set(String::NewFromUtf8(isolate, "RESTART"), Number::New(isolate, LUA_GCRESTART));
    constants->Set(String::NewFromUtf8(isolate, "COLLECT"), Number::New(isolate, LUA_GCCOLLECT));
    constants->Set(String::NewFromUtf8(isolate, "COUNT"), Number::New(isolate, LUA_GCCOUNT));
    constants->Set(String::NewFromUtf8(isolate, "COUNTB"), Number::New(isolate, LUA_GCCOUNTB));
    constants->Set(String::NewFromUtf8(isolate, "STEP"), Number::New(isolate, LUA_GCSTEP));
    constants->Set(String::NewFromUtf8(isolate, "SETPAUSE"), Number::New(isolate, LUA_GCSETPAUSE));
    constants->Set(String::NewFromUtf8(isolate, "SETSTEPMUL"), Number::New(isolate, LUA_GCSETSTEPMUL));
    exports->Set(String::NewFromUtf8(isolate, "GC"), constants);
}


void init(Handle<Object> exports) {
    LuaState::Init(exports);
    init_gc_constants(exports);
    init_status_constants(exports);
    init_info_constants(exports);
}

NODE_MODULE(luanode, init)
