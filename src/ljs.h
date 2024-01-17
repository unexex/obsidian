#ifdef __EMSCRIPTEN__ 
#include <emscripten.h>

int runJSString(const char* js) {
    return EM_ASM_INT({
        try {
            eval(UTF8ToString($0));
            return 0;
        } catch (e) {
            return 1;
        }
    }, js);
}
#else
int runJSString(const char* js) {
    return 1;
}
#endif