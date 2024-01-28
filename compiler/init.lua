--[[
* Lua (Obsidian) to WebAssembly (WALT) compiler
* Author: Aarav Sethi
* This work is licensed under the MIT license
*
* This is a very straightfoward compiler, it uses the Obsidian VM parser and runs through a visitor pattern to compile the code.
]]

local binaryen = js.import("binaryen")
local visitors = {
    -- for i, v in pairs(node) do print(i, v) end
    CallStatement = function(node)
        local line = ""
        for i, v in pairs(node.expression.arguments) do 
            if tonumber(i) then
                line = line .. visit(v) .. "\n"
            end
        end
        line = line .. "call $" .. node.expression.base.name
        return line
    end,
    StringLiteral = function(node) -- Todo memory & strings
        table.insert(memory, node.raw)
        --[[return ()]]
        return ""
    end,
    FunctionDeclaration = function(node)
        local line = ""
        line = line .. "(func $" .. node.identifier.name .. "\n"
        for i, v in pairs(node.body) do
            line = line .. visit(v) .. "\n"
        end
        line = line .. ")"
        table.insert(functions,line)
        return ""
    end,
}


function visit(node)
    local visitor = visitors[node.type]
    if visitor then
        return visitor(node)
    else
        error("Unknown node type: " .. node.type)
    end
end

function compile(code)
    local ast = ob.parse(code).body
    for _, node in pairs(ast) do
        compiled = compiled .. visit(node) .. "\n"
    end
end

print(compile([[
function myFunction()
    print("Hello World!")
end
myFunction()
]]))