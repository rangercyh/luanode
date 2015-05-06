#ifndef LUASTATE_H
#define LUASTATE_H

#include <node.h>
#include <node_object_wrap.h>

#include "values.h"

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaState : public node::ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit inline LuaState() {}
    inline ~LuaState() {}

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetGlobal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetGlobal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Push(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Pop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetTop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetTop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Replace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CollectGarbage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Status(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void DoFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void DoString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CallFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    // static v8::Persistent<v8::Function> constructor;
    lua_State* lua_;
    char* name_;
};

#endif
