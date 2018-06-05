addon = require('./build/Release/addon.node')

exports.call = function (...args)
{
    response = addon.call(...args);
    if ("result" in response)
    {
        return response["result"]
    }

    return {error: response["error"]}
}

function export_methods()
{
    let methods = exports.call("methods");
    for (let i = 0; i < methods.length; i++) {
        let m = methods[i];
        exports[m] = (...args) => exports.call(m, ...args);
    }
}

function clone(obj)
{
    return JSON.parse(JSON.stringify(obj));
}

function deepFreeze(o)
{
    Object.freeze(o);
    Object.getOwnPropertyNames(o).forEach(function (prop) {
        let proceed = o.hasOwnProperty(prop) && o[prop] != null && (typeof o[prop] === "object" || typeof o[prop] === "function");
        if (proceed) 
        {
            deepFreeze(o[prop]);
        }
    });
    return o;
}

function export_operations()
{
    let raw_ops = exports.call("operations");
    let ops = {};
    for (let i = 0; i < raw_ops.length; ++i)
    {
        let op = raw_ops[i];
        let name = op[0].replace("_operation", "");
        let obj = op[1];
        obj.clone = () => {return clone(obj)};
        ops[name] = obj;
    }

    exports.operations = deepFreeze(ops);
}

function export_templates()
{
    let raw = exports.call("templates");
    let templates = {};
    for (let i = 0; i < raw.length; ++i)
    {
        let t = raw[i];
        let obj = t[1];
        obj.clone = () => {return clone(obj);};
        templates[t[0]] = t[1];
    }
    exports.templates = deepFreeze(templates);
}

exports.attach = function (addr)
{
    addon.attach(addr);
    export_methods();
    export_operations();
    export_templates();
    return exports;
}

// exports.attach("ws://localhost:13010")

