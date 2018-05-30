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

exports.abi = function (abi)
{
    abi['at'] = function(account, contract_id)
    {
        let contract = {signatures:[]};
        for (let i = 0; i < abi.length; i++)
        {
            let name = abi[i].name;
            let signature = name + "(";
            if ("inputs" in abi[i])
            {
                let inputs = abi[i].inputs;
                let arg_count = inputs.length;
                for (let j = 0; j < arg_count; ++j)
                {
                    let arg = inputs[j];

                    signature += arg.type;
                    if (j < arg_count - 1)
                    {
                        signature += ",";
                    }
                }
            }

            signature += ')';
            contract.signatures.push(signature);

            contract[name] = () => exports.call("call_contract", contract_id, account, [signature]);
        }

        return contract;
    }

    return abi;
}

exports.abi_from_file = function (path)
{
    let fs = require('fs');
    let contents = fs.readFileSync(path, 'utf8');
    let abi = JSON.parse(contents);
    return exports.abi(JSON.parse(contents));

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

