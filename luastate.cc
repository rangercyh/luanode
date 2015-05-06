#include "luastate.h"

using namespace v8;

void LuaState::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "LuaState"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "setGlobal", SetGlobal);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getGlobal", GetGlobal);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getName", GetName);
    NODE_SET_PROTOTYPE_METHOD(tpl, "push", Push);
    NODE_SET_PROTOTYPE_METHOD(tpl, "pop", Pop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getTop", GetTop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setTop", SetTop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "replace", Replace);
    NODE_SET_PROTOTYPE_METHOD(tpl, "status", Status);
    NODE_SET_PROTOTYPE_METHOD(tpl, "collectGarbage", CollectGarbage);
    NODE_SET_PROTOTYPE_METHOD(tpl, "doFile", DoFile);
    NODE_SET_PROTOTYPE_METHOD(tpl, "doString", DoString);
    NODE_SET_PROTOTYPE_METHOD(tpl, "call", CallFunction);

    exports->Set(String::NewFromUtf8(isolate, "LuaState"), tpl->GetFunction());
}

void LuaState::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (!args.IsConstructCall()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState Requires The 'new' Operator To Create An Instance")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!(args.Length() > 0)) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState First Argument Must Be A String")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    LuaState* obj = new LuaState();
    obj->name_ = get_lua_str(args[0]);
    obj->lua_ = lua_open();
    luaL_openlibs(obj->lua_);
    obj->Wrap(args.This());

    args.GetReturnValue().Set(args.This());
}

void LuaState::GetName(const FunctionCallbackInfo<Value>& args){
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, obj->name_));
}

void LuaState::DoFile(const FunctionCallbackInfo<Value>& args){
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.doFileSync Takes Only 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.doFileSync Argument 1 Must Be A String")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    char* file_name = get_lua_str(args[0]);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    if (luaL_dofile(obj->lua_, file_name)) {
        char buf[1000];
        sprintf(buf, "Exception Of File %s Has Failed:\n%s\n", file_name, lua_tostring(obj->lua_, -1));
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, buf)));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (lua_gettop(obj->lua_)) {
        args.GetReturnValue().Set(lua_to_js_value(obj->lua_, -1));
    } else {
        args.GetReturnValue().SetUndefined();
    }
}

void LuaState::DoString(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.doStringSync Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    char* lua_code = get_lua_str(args[0]);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    if (luaL_dostring(obj->lua_, lua_code)) {
        char buf[1000];
        sprintf(buf, "Execution Of Lua Code Has Failed:\n%s\n", lua_tostring(obj->lua_, -1));
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, buf)));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (lua_gettop(obj->lua_)) {
        args.GetReturnValue().Set(lua_to_js_value(obj->lua_, -1));
    } else {
        args.GetReturnValue().SetUndefined();
    }
}

void LuaState::CallFunction(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.CallFunction Requires At Least 1 Arguments")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    char* fname = get_lua_str(args[0]);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_getglobal(obj->lua_, fname);

    int argnum = 0;
    for (int i = 1; i < args.Length(); ++i, ++argnum) {
        push_js_value_to_lua(obj->lua_, args[i]);
    }

    if (lua_pcall(obj->lua_, argnum, 1, 0) != 0) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.CallFunction error")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    args.GetReturnValue().Set(lua_to_js_value(obj->lua_, -1));
}

void LuaState::SetGlobal(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 2) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.setGlobal Requires 2 Arguments")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.setGlobal Argument 1 Must Be A String")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    char* global_name = get_lua_str(args[0]);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    push_js_value_to_lua(obj->lua_, args[1]);
    lua_setglobal(obj->lua_, global_name);
}

void LuaState::GetGlobal(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.getGlobal Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.getGlobal Argument 1 Must Be A String")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    char* global_name = get_lua_str(args[0]);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_getglobal(obj->lua_, global_name);
    args.GetReturnValue().Set(lua_to_js_value(obj->lua_, -1));
}

void LuaState::Close(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_close(obj->lua_);
}

void LuaState::Status(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    args.GetReturnValue().Set(Number::New(isolate, lua_status(obj->lua_)));
}

void LuaState::CollectGarbage(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.collectGarbageSync Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaSatte.collectGarbageSync Argument 1 Must Be A Number, try LuaState.GC.[TYPE]")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    args.GetReturnValue().Set(Number::New(isolate, lua_gc(obj->lua_, (int)args[0]->ToNumber()->Value(), 0)));
}

void LuaState::Push(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.push Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    push_js_value_to_lua(obj->lua_, args[0]);
}

void LuaState::Pop(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    int pop_n = 1;
    if (args.Length() > 0 && args[0]->IsNumber()) {
        pop_n = (int)args[0]->ToNumber()->Value();
    }

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_pop(obj->lua_, pop_n);
}

void LuaState::GetTop(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    args.GetReturnValue().Set(Number::New(isolate, lua_gettop(obj->lua_)));
}

void LuaState::SetTop(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    int set_n = 0;
    if (args.Length() > 0 && args[0]->IsNumber()) {
        set_n = (int)args[0]->ToNumber()->Value();
    }

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_settop(obj->lua_, set_n);
}

void LuaState::Replace(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1){
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.replace Requires 1 Argument")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    if (!args[0]->IsNumber()){
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
            "LuaState.replace Argument 1 Must Be A Number")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
    lua_replace(obj->lua_, (int)args[0]->ToNumber()->Value());
}
