#include <node.h>
#include "appwallet.hpp"
#include <iostream>

namespace demo 
{

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Exception;
using v8::Array;
using v8::Function;

graphene::wallet::appwallet* wallet = nullptr;

std::string to_stdstr(const Local<String>& str)
{
    String::Utf8Value value(str);
    return *value;
}

std::string stringify(Isolate* isolate, Local<Value> value)
{
    if (value.IsEmpty())
    {
        return {};
    }

    v8::HandleScope scope(isolate);
    auto json = isolate->GetCurrentContext()->Global()->Get(String::NewFromUtf8(isolate, "JSON"))->ToObject();
    auto func = json->Get(String::NewFromUtf8(isolate, "stringify")).As<v8::Function>();

    auto result = func->Call(json, 1, &value);

    String::Utf8Value str(result);
    return {*str, str.length()};
}

void hello(const FunctionCallbackInfo<Value>& args) 
{
    Isolate* isolate = args.GetIsolate();
    auto prop = wallet->test();
    Local<String> str = String::NewFromUtf8(isolate, prop.c_str());
    args.GetReturnValue().Set(v8::JSON::Parse(str));
}

void call(const FunctionCallbackInfo<Value>& args) 
{
    Isolate* isolate = args.GetIsolate();
    if (args.Length() < 1)
    {
        auto msg = String::NewFromUtf8(isolate, "Wrong number of argumantes");
        isolate->ThrowException(Exception::TypeError(msg));
    }

    auto arr = Array::New(isolate);
    for (auto i = 1; i <  args.Length(); ++i)
    {
        arr->Set(i - 1, args[i]);
    }

    auto method_name = args[0]->ToString();
    auto request = std::string("{\"jsonrpc\":\"2.0\",\"method\":\"")
        .append(to_stdstr(method_name))
        .append("\",\"params\":")
        .append(stringify(isolate, arr))
        .append(",\"id\":1}");

    auto response = wallet->call(request);
    auto response_str = String::NewFromUtf8(isolate, response.c_str());
    args.GetReturnValue().Set(v8::JSON::Parse(response_str));
}

void attach(const FunctionCallbackInfo<Value>& args) 
{
    Isolate* isolate = args.GetIsolate();
    if (args.Length() < 1)
    {
        auto msg = String::NewFromUtf8(isolate, "Wrong number of argumantes");
        isolate->ThrowException(Exception::TypeError(msg));
    }
    auto addr = args[0]->ToString();
    wallet->connect(to_stdstr(addr));
}

void init(Local<Object> exports) 
{
      wallet = new graphene::wallet::appwallet;

      NODE_SET_METHOD(exports, "hello", hello);
      NODE_SET_METHOD(exports, "call", call);
      NODE_SET_METHOD(exports, "attach", attach);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)

}  // namespace demo
