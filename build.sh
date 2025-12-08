#!/bin/sh

seq 1 13 | awk '{
    num_states = $1
    printf "#define TOKEN_DEFINE_%02d(token, str) \\\n", num_states
    print  "static sts_t token(const char c, uint8_t *const s) \\"
    print  "{ \\"
    print  "    switch (*s) { \\"

    for (idx = 0; idx < num_states; idx++) {
        state = (idx == num_states - 1 ? "ACCEPT" : "HUNGRY")
        printf "    case %2d: return c == (str)[%2d] ? TR(%2d, %s) : REJECT; \\\n", idx, idx, idx + 1, state
    }

    printf "    case %2d: return REJECT; \\\n", idx
    print  "    default: assert(false); __builtin_unreachable(); \\"
    print  "    } \\"
    print  "}\n"
}' > tk-defines.inc

if [ -z "$DEBUG" ]; then
    DEBUG=1
fi

CC=clang
CFLAGS="-std=gnu23 -pedantic -Wall -Wextra "

if [ "$CC" = "clang" ]; then
    CFLAGS+="-Wno-gnu-statement-expression-from-macro-expansion "
    CFLAGS+="-Wno-gnu-conditional-omitted-operand "
    CFLAGS+="-Wno-gnu-designator "
    CFLAGS+="-Wno-gnu-case-range "
fi

if [ "$DEBUG" = "1" ]; then
    CFLAGS+="-O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer"
else
    CFLAGS+="-Werror -DNDEBUG -O3 -flto -fomit-frame-pointer"
fi

set -x
$CC cdecl-dump.c $CFLAGS -o cdecl-dump