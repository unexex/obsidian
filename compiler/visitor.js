import binaryen from "binaryen";

const variables = {};

const nodes = {
    /* Functions (Call and Declaration and Embedded Declaration) */
    CallStatement: function(Module, node){
        const name = node.expression.base.name;
        const args = node.expression.arguments;

        Module.call(name, [binaryen.anyref], binaryen.none);
    },

    /* Control Flow (If, While, For) */
    IfStatement: function(Module, node){
        const clauses = node.clauses;
        // TODO: support multiple clauses
        const clause = clauses[0];
        const condition = clause.condition;
        const body = clause.body;
        console.log(condition, body)

        Module.if(
            Module.i64.eq(
                generate(Module, condition),
                Module.i32.const(1)
            ),
            generate(Module, body)
        );
    },

    /* Expressions (Binary, Unary, Literal, Identifier) */
    

    /* Literals (Boolean, Number, String) */
    NumericLiteral: function(Module, node){
        return Module.i64.const(Number(node.raw));
    },
    BooleanLiteral: function(Module, node){
        if (node.value === true) {
            return Module.i64.const(1);
        }
        return Module.i64.const(0);
    },
}
function generate(module, node){
    if (node.type in nodes) {
        return nodes[node.type](module, node);
    } else {
        //throw new Error("Unknown node type: " + type);
    }
}
function visitTree(module, tree){
    for (var i = 0; i < tree.length; i++) {
        var node = tree[i];
        generate(module, node);
    }
}
export default function visit(module, ast) {
    visitTree(module, ast.body);
}