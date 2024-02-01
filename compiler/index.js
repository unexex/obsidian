import binaryen from "binaryen";
import wizer from "@bytecodealliance/wizer";
import visit from "./visitor";
import oparser from "../src/oparser";

const execFile = require('child_process').execFile;

/* PARSE INPUT */
var input, output, style, useWizer;
for (var i = 0; i < process.argv.length; i++) {
    var arg = process.argv[i];
    if (arg === "-o") {
        output = process.argv[++i];
    } else if (arg === "-s") {
        style = process.argv[++i];
    } else if (arg === "-p") {
        useWizer = process.argv[++i];
    } else {
        input = process.argv[i];
    }
}
if (!input) {
    console.error("Missing input");
    process.exit(1);
}
if (!output) {
    console.error("Missing output");
    process.exit(1);
}
if (!style) {
    style = "wasm";
}
if (style != "wasm" && style != "wat") {
    console.error("Invalid style");
    process.exit(1);
}

/* RUN COMPILE */
var Module = new binaryen.Module();
var inputcontent = require("fs").readFileSync(input, "utf8");
var ast = oparser.parse(inputcontent);

console.log(ast);

visit(Module, ast);

//Module.optimize();

//if (!Module.validate())
//  throw new Error("validation error");

var textData = Module.emitText();
var wasmData = Module.emitBinary();

/* WRITE OUTPUT */
if (style == "wat") {
    require("fs").writeFileSync(output, textData);
}else{
    require("fs").writeFileSync(output, wasmData);
}
if (useWizer) {
    if (style == "wat"){
        throw new Error("Wizer does not support wat output")
    }
    execFile(wizer, [output, '-o', useWizer], (err, stdout) => {
        console.log(stdout);
    });
}