#include <stdlib.h>
#include "values.h"

using namespace v8;

char* get_lua_str(Local<Value> val) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (!val->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument Must Be A String")));
        return NULL;
    }

    String::Utf8Value val_string(val);
    char* val_char_ptr = (char *) malloc(val_string.length() + 1);
    strcpy(val_char_ptr, *val_string);
    return val_char_ptr;
}

Local<Value> lua_to_js_value(lua_State* L, int i) {
    Isolate* isolate = Isolate::GetCurrent();
    // HandleScope scope(isolate);

    switch (lua_type(L, i)) {
    case LUA_TBOOLEAN:
        return Local<Boolean>::New(isolate, Boolean::New(isolate, lua_toboolean(L, i)));
    case LUA_TNUMBER:
        return Local<Number>::New(isolate, Number::New(isolate, lua_tonumber(L, i)));
    case LUA_TSTRING:
        return String::NewFromUtf8(isolate, (char *)lua_tostring(L, i));
    case LUA_TTABLE:
    {
        Local<Object> obj = Object::New(isolate);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            Local<Value> key = lua_to_js_value(L, -2);
            Local<Value> value = lua_to_js_value(L, -1);
            obj->Set(key, value);
            lua_pop(L, 1);
        }
        return obj;
    }
    default:
        return Local<Primitive>::New(isolate, Undefined(isolate));
    }
}

void push_js_value_to_lua(lua_State* L, Handle<Value> value) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (value->IsString()) {
        lua_pushstring(L, get_lua_str(Local<Value>::New(isolate, value)));
    } else if(value->IsNumber()) {
        int i_value = (int)value->ToNumber()->Value();
        lua_pushinteger(L, i_value);
    } else if (value->IsBoolean()) {
        int b_value = (int)value->ToBoolean()->Value();
        lua_pushboolean(L, b_value);
    } else if (value->IsObject()) {
        lua_newtable(L);
        Local<Object> obj = value->ToObject();
        Local<Array> keys = obj->GetPropertyNames();
        for (uint32_t i = 0; i < keys->Length(); ++i) {
            Local<Value> key = keys->Get(i);
            Local<Value> val = obj->Get(key);
            // 对于js的数组转换成lua的table，把key从1开始，而不是0
            if (value->IsArray() && key->IsNumber()) {
                key = Number::New(isolate, key->ToNumber()->Value() + 1);
            }
            push_js_value_to_lua(L, key);
            push_js_value_to_lua(L, val);
            lua_settable(L, -3);
        }
    } else {
        lua_pushnil(L);
    }
}
