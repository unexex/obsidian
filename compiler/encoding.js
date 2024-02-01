/*
Details:
- Data is stored as a i64 regardless of the type
- Lua Numbers are stored as they are raw
- Lua Booleans are stored as 1 or 0
- Lua Strings uses a custom string encoding
*/
export function encodeString(string) {
    let result = [];
    for (let i = 0; i < string.length; i++) {
        result.push(string.charCodeAt(i));
    }
    return result;
}