#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <assert.h>

#ifdef NO_COLOUR
#define COLOURED(s, b, c) s
#else
#define COLOURED(s, b, c) "\033[" #b ";" #c "m" s "\033[0m"
#endif

#define RED(s)      COLOURED(s, 0, 91)
#define GREEN(s)    COLOURED(s, 0, 92)
#define YELLOW(s)   COLOURED(s, 0, 93)
#define BLUE(s)     COLOURED(s, 0, 94)
#define PURPLE(s)   COLOURED(s, 0, 95)
#define CYAN(s)     COLOURED(s, 0, 96)
#define WHITE(s)    COLOURED(s, 0, 97)

#define B_RED(s)    COLOURED(s, 1, 91)
#define B_GREEN(s)  COLOURED(s, 1, 92)
#define B_YELLOW(s) COLOURED(s, 1, 93)
#define B_BLUE(s)   COLOURED(s, 1, 94)
#define B_PURPLE(s) COLOURED(s, 1, 95)
#define B_CYAN(s)   COLOURED(s, 1, 96)
#define B_WHITE(s)  COLOURED(s, 1, 97)

#define S_WHITE(s)  COLOURED(s, 4, 97)

enum {
    /* Whitespace */
    TK_WSPC,

    /* Unrecognised name or identifier */
    TK_NAME,

    /* Decimal whole number for array count */
    TK_NMBR,

    /* Storage class specifiers: auto, register, static, extern, _Thread_local, typedef */
    TK_AUTO, TK_REGI, TK_STAT, TK_EXTE, TK_THRL, TK_TYPE,

    /* Type qualifiers: const, volatile, restrict, _Atomic */
    TK_CONS, TK_VOLA, TK_REST, TK_ATOM,

    /* Keywords: struct, enum, void */
    TK_STRU, TK_ENUM, TK_VOID,

    /* Type specifiers: char, int, float, double, signed, unsigned, short, long */
    TK_TCHR, TK_TINT, TK_TFLT, TK_TDBL, TK_TSGN, TK_TUNS, TK_TSHR, TK_TLON,

    /* Star, left/right paren, left/right bracket */
    TK_STAR, TK_LPAR, TK_RPAR, TK_LBRA, TK_RBRA, TK_COMA,

    /* Count of all tokens */
    TK_COUNT,

    /* Special markers for line beginning and end */
    TK_LBEG, TK_LEND,
};

typedef uint8_t tk_t;

struct token {
    const char *beg, *end;
    tk_t tk;
};

enum {
    LEX_OK, LEX_BAD, LEX_NOMEM,
};

enum {
    STS_ACCEPT, STS_REJECT, STS_HUNGRY,
};

typedef uint8_t sts_t;

#define TR(st, tr) (*s = (st), (STS_##tr))
#define REJECT TR(0, REJECT)

#define is_alpha(c) ({ \
    const char ch = (c); \
    (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); \
})

#define is_digit(c) ({ \
    const char ch = (c); \
    ch >= '0' && ch <= '9'; \
})

#define is_alnum(c) ({ \
    const char ch = (c); \
    (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'); \
})

#define is_wspace(c) ({ \
    const char ch = (c); \
    ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'; \
})

static sts_t tk_wspc(const char c, uint8_t *const s)
{
    enum {
        tk_wspc_begin,
        tk_wspc_accum,
    };

    switch (*s) {
    case tk_wspc_begin:
        return is_wspace(c) ? TR(tk_wspc_accum, ACCEPT) : REJECT;

    case tk_wspc_accum:
        return is_wspace(c) ? STS_ACCEPT : REJECT;
    }

    __builtin_unreachable();
    assert(false);
}

static sts_t tk_name(const char c, uint8_t *const s)
{
    enum {
        tk_name_begin,
        tk_name_accum,
    };

    switch (*s) {
    case tk_name_begin:
        return is_alpha(c) || c == '_' ? TR(tk_name_accum, ACCEPT) : REJECT;

    case tk_name_accum:
        return is_alnum(c) || c == '_' ? STS_ACCEPT : REJECT;
    }

    __builtin_unreachable();
    assert(false);
}

static sts_t tk_nmbr(const char c, uint8_t *const s)
{
    enum {
        tk_nmbr_begin,
        tk_nmbr_accum,
    };

    switch (*s) {
    case tk_nmbr_begin:
        return is_digit(c) && c != '0' ? TR(tk_nmbr_accum, ACCEPT) : REJECT;

    case tk_nmbr_accum:
        return is_digit(c) ? STS_ACCEPT : REJECT;
    }

    __builtin_unreachable();
    assert(false);
}

#include "tk-defines.inc"

TOKEN_DEFINE_04(tk_auto, "auto")
TOKEN_DEFINE_08(tk_regi, "register")
TOKEN_DEFINE_06(tk_stat, "static")
TOKEN_DEFINE_06(tk_exte, "extern")
TOKEN_DEFINE_13(tk_thrl, "_Thread_local")
TOKEN_DEFINE_07(tk_type, "typedef")

TOKEN_DEFINE_05(tk_cons, "const")
TOKEN_DEFINE_08(tk_vola, "volatile")
TOKEN_DEFINE_08(tk_rest, "restrict")
TOKEN_DEFINE_07(tk_atom, "_Atomic")

TOKEN_DEFINE_06(tk_stru, "struct")
TOKEN_DEFINE_04(tk_enum, "enum")
TOKEN_DEFINE_04(tk_void, "void")

TOKEN_DEFINE_04(tk_tchr, "char")
TOKEN_DEFINE_03(tk_tint, "int")
TOKEN_DEFINE_05(tk_tflt, "float")
TOKEN_DEFINE_06(tk_tdbl, "double")
TOKEN_DEFINE_06(tk_tsgn, "signed")
TOKEN_DEFINE_08(tk_tuns, "unsigned")
TOKEN_DEFINE_05(tk_tshr, "short")
TOKEN_DEFINE_04(tk_tlon, "long")

TOKEN_DEFINE_01(tk_star, "*")
TOKEN_DEFINE_01(tk_lpar, "(")
TOKEN_DEFINE_01(tk_rpar, ")")
TOKEN_DEFINE_01(tk_lbra, "[")
TOKEN_DEFINE_01(tk_rbra, "]")
TOKEN_DEFINE_01(tk_coma, ",")

static sts_t (*const token_funcs[TK_COUNT])(const char, uint8_t *const) = {
    tk_wspc,
    tk_name,
    tk_nmbr,
    tk_auto, tk_regi, tk_stat, tk_exte, tk_thrl, tk_type,
    tk_cons, tk_vola, tk_rest, tk_atom,
    tk_stru, tk_enum, tk_void,
    tk_tchr, tk_tint, tk_tflt, tk_tdbl, tk_tsgn, tk_tuns, tk_tshr, tk_tlon,
    tk_star, tk_lpar, tk_rpar, tk_lbra, tk_rbra, tk_coma,
};

static inline int push_token(struct token **const tokens,
    size_t *const ntokens, size_t *const allocated, const tk_t token,
    const char *const beg, const char *const end)
{
    if (*ntokens >= *allocated) {
        *allocated = (*allocated ?: 1) * 8;
        struct token *const tmp = realloc(*tokens, *allocated * sizeof(struct token));

        if (!tmp) {
            free(*tokens);
            *tokens = NULL;
            return LEX_NOMEM;
        }

        *tokens = tmp;
    }

    (*tokens)[(*ntokens)++] = (struct token) {
        .beg = beg,
        .end = end,
        .tk = token
    };

    return LEX_OK;
}

static int lex(const char *const input, struct token **const tokens, size_t *const ntokens)
{
    static struct {
        sts_t prev, curr;
    } statuses[TK_COUNT] = {
        [0 ... TK_COUNT - 1] = { STS_HUNGRY, STS_REJECT }
    };

    uint8_t states[TK_COUNT] = {0};

    const char *prefix_beg = input, *prefix_end = input;
    tk_t accepted_token;
    size_t allocated = 0;
    *tokens = NULL, *ntokens = 0;

    #define PUSH_OR_NOMEM(tk, beg, end) \
        if (push_token(tokens, ntokens, &allocated, (tk), (beg), (end))) { \
            return LEX_NOMEM; \
        }

    #define foreach_tk for (tk_t tk = 0; tk < TK_COUNT; ++tk)

    PUSH_OR_NOMEM(TK_LBEG, NULL, NULL);

    while (*prefix_end) {
        bool did_accept = false;

        foreach_tk {
            if (statuses[tk].prev != STS_REJECT) {
                statuses[tk].curr = token_funcs[tk](*prefix_end, &states[tk]);
            }

            if (statuses[tk].curr != STS_REJECT) {
                did_accept = true;
            }
        }

        if (did_accept) {
            prefix_end++;

            foreach_tk {
                statuses[tk].prev = statuses[tk].curr;
            }
        } else {
            accepted_token = TK_COUNT;

            foreach_tk {
                if (statuses[tk].prev == STS_ACCEPT) {
                    accepted_token = tk;
                }

                statuses[tk].prev = STS_HUNGRY;
                statuses[tk].curr = STS_REJECT;
            }

            PUSH_OR_NOMEM(accepted_token, prefix_beg, prefix_end);

            if (accepted_token == TK_COUNT) {
                (*tokens)[*ntokens - 1].end++;
                return LEX_BAD;
            }

            prefix_beg = prefix_end;
        }
    }

    accepted_token = TK_COUNT;

    foreach_tk {
        if (statuses[tk].curr == STS_ACCEPT) {
            accepted_token = tk;
        }

        statuses[tk].prev = STS_HUNGRY;
        statuses[tk].curr = STS_REJECT;
    }

    PUSH_OR_NOMEM(accepted_token, prefix_beg, prefix_end);

    if (accepted_token == TK_COUNT) {
        return LEX_BAD;
    }

    PUSH_OR_NOMEM(TK_LEND, NULL, NULL);
    return LEX_OK;

    #undef PUSH_OR_NOMEM
    #undef foreach_tk
}

static void print_lex_error(const struct token *const tokens, const size_t ntokens)
{
    for (size_t i = 0, total_len = 0; i < ntokens; ++i) {
        const struct token token = tokens[i];

        if (token.tk == TK_LBEG || token.tk == TK_LEND) {
            continue;
        }

        const int len = token.end - token.beg;
        total_len += len;

        if (i == ntokens - 1) {
            fprintf(stderr, YELLOW("%.*s") "\n", len ?: 1, token.beg);

            for (size_t j = 0; j < total_len; ++j) {
                fputc(' ', stderr);
            }

            fputs(RED("\\ Unrecognised character") "\n", stderr);
        } else {
            fprintf(stderr, "%.*s", len, token.beg);
        }
    }
}

enum {
    NT_Dtop, /* Top-level reduction */
    NT_Full, /* Full declaration which is a specifier-qualifier list, optionally followed by a declarator */
    NT_Squl, /* Specifier-qualifier list */
    NT_Squa, /* Specifier or qualifier */
    NT_Decl, /* Declarator */

    NT_Ndcl, /* Name declarator */
    NT_Adcl, /* Array declarator */
    NT_Bdcl, /* Abstract array declarator */
    NT_Fdcl, /* Function declarator */
    NT_Gdcl, /* Parenthesised declarator for grouping */
    NT_Hdcl, /* Abstract function declarator */
    NT_Pdcl, /* Pointer declarator */
    NT_Qdcl, /* Pointer declarator with qualifications */
    NT_Rdcl, /* Abstract pointer declarator with qualifications */
    NT_Sdcl, /* Abstract pointer declarator */

    NT_List, /* Comma-delimited parameter list */

    NT_COUNT
};

typedef uint8_t nt_t;

struct token;
struct node {
    /* use "token" if nchildren == 0, "nt" and "children" otherwise */
    size_t nchildren;

    union {
        const struct token *token;

        struct {
            nt_t nt;
            struct node **children;
        };
    };
};

enum {
    PARSE_OK,
    PARSE_REJECT,
    PARSE_NOMEM,
};

#define parse_error(root) ({ \
    struct node root_once = (root); \
    root_once.nchildren ? PARSE_OK : root_once.token->tk; \
})

#define skip_token(t) ({ \
    const tk_t t_once = (t); \
    t_once == TK_WSPC; \
})

#define RULE_RHS_LAST 4
#define GRAMMAR_SIZE (sizeof(grammar) / sizeof(*grammar))

#define n(_nt) { .nt = NT_##_nt, .is_tk = 0, .is_mt = 0 }
#define m(_nt) { .nt = NT_##_nt, .is_tk = 0, .is_mt = 1 }
#define t(_tm) { .tk = TK_##_tm, .is_tk = 1, .is_mt = 0 }
#define no     { .tk = TK_COUNT, .is_tk = 1, .is_mt = 0 }

#define r1(_lhs, t1) \
    { .lhs = NT_##_lhs, .rhs = { no, no, no, no, t1, } },
#define r2(_lhs, t1, t2) \
    { .lhs = NT_##_lhs, .rhs = { no, no, no, t1, t2, } },
#define r3(_lhs, t1, t2, t3) \
    { .lhs = NT_##_lhs, .rhs = { no, no, t1, t2, t3, } },
#define r4(_lhs, t1, t2, t3, t4) \
    { .lhs = NT_##_lhs, .rhs = { no, t1, t2, t3, t4, } },

static const struct rule {
    /* left-hand side of production */
    const nt_t lhs;

    /* array of RULE_RHS_LAST + 1 terms which form the right-hand side */
    const struct term {
        /* a rule RHS term is either a terminal token or a non-terminal */
        union {
            const tk_t tk;
            const nt_t nt;
        };

        /* indicates which field of the above union to use */
        const uint8_t is_tk: 1;

        /* indicates that the non-terminal can be matched multiple times */
        const uint8_t is_mt: 1;
    } rhs[RULE_RHS_LAST + 1];
} grammar[] = {
    r3(Dtop, t(LBEG), n(Full), t(LEND)           )

    r1(Squl, m(Squa)                             )

    r1(Squa, t(AUTO)                             )
    r1(Squa, t(REGI)                             )
    r1(Squa, t(STAT)                             )
    r1(Squa, t(EXTE)                             )
    r1(Squa, t(THRL)                             )
    r1(Squa, t(TYPE)                             )

    r1(Squa, t(CONS)                             )
    r1(Squa, t(VOLA)                             )
    r1(Squa, t(REST)                             )
    r1(Squa, t(ATOM)                             )

    r2(Squa, t(STRU), t(NAME)                    )
    r2(Squa, t(ENUM), t(NAME)                    )
    r1(Squa, t(VOID)                             )

    r1(Squa, t(TCHR)                             )
    r1(Squa, t(TINT)                             )
    r1(Squa, t(TFLT)                             )
    r1(Squa, t(TDBL)                             )
    r1(Squa, t(TSGN)                             )
    r1(Squa, t(TUNS)                             )
    r1(Squa, t(TSHR)                             )
    r1(Squa, t(TLON)                             )

    r1(Decl, n(Ndcl)                             )
    r1(Decl, n(Adcl)                             )
    r1(Decl, n(Bdcl)                             )
    r1(Decl, n(Fdcl)                             )
    r1(Decl, n(Gdcl)                             )
    r1(Decl, n(Hdcl)                             )
    r1(Decl, n(Pdcl)                             )
    r1(Decl, n(Qdcl)                             )
    r1(Decl, n(Rdcl)                             )
    r1(Decl, n(Sdcl)                             )

    r1(Ndcl, t(NAME)                             )

    r4(Adcl, n(Decl), t(LBRA), t(NMBR), t(RBRA)  )
    r3(Bdcl, t(LBRA), t(NMBR), t(RBRA)           )

    r4(Fdcl, n(Decl), t(LPAR), n(Full), t(RPAR)  )
    r4(Fdcl, n(Decl), t(LPAR), n(List), t(RPAR)  )
    r3(Gdcl, t(LPAR), n(Decl), t(RPAR)           )
    r3(Hdcl, t(LPAR), n(Full), t(RPAR)           )
    r3(Hdcl, t(LPAR), n(List), t(RPAR)           )

    r2(Pdcl, t(STAR), n(Decl)                    )
    r3(Qdcl, t(STAR), n(Squl), n(Decl)           )
    r2(Rdcl, t(STAR), n(Squl)                    )
    r1(Sdcl, t(STAR)                             )

    r3(List, n(Full), t(COMA), n(Full)           )
    r3(List, n(List), t(COMA), n(List)           )
    r3(List, n(List), t(COMA), n(Full)           )

    r2(Full, n(Squl), n(Decl)                    )
    r1(Full, n(Squl)                             )
};

#undef r1
#undef r2
#undef r3
#undef r4

#undef n
#undef m
#undef t
#undef no

static inline bool should_shift(const nt_t lhs, const tk_t ahead)
{
    switch (lhs) {
    case NT_Full:
        switch (ahead) {
        case TK_LEND:
        case TK_RPAR:
        case TK_COMA:
            break;

        default:
            return true;
        }
        break;

    case NT_Squl:
        switch (ahead) {
        case TK_AUTO ... TK_TLON:
            return true;
        }
        break;

    case NT_Rdcl:
    case NT_Sdcl:
        switch (ahead) {
        case TK_AUTO ... TK_TLON:
        case TK_STAR:
            return true;

        case TK_LPAR:
        case TK_LBRA:
        case TK_NAME:
            return true;
        }
        break;

    case NT_Pdcl:
    case NT_Qdcl:
        switch (ahead) {
        case TK_LPAR:
        case TK_LBRA:
            return true;
        }
        break;
    }

    return false;
}

static struct {
    size_t size, allocated;
    struct node *nodes;
} stack;

#ifndef NDEBUG
static void print_stack(void)
{
    static const char *const nts[NT_COUNT] = {
        "Dtop",
        "Full",
        "Squl",
        "Squa",
        "Decl",
        "Ndcl",
        "Adcl",
        "Bdcl",
        "Fdcl",
        "Gdcl",
        "Hdcl",
        "Pdcl",
        "Qdcl",
        "Rdcl",
        "Sdcl",
        "List",
    };

    for (size_t i = 0; i < stack.size; ++i) {
        const struct node *const node = &stack.nodes[i];

        if (node->nchildren) {
            printf(YELLOW("%s "), nts[node->nt]);
        } else if (node->token->tk == TK_LBEG) {
            printf(GREEN("^ "));
        } else if (node->token->tk == TK_LEND) {
            printf(GREEN("$ "));
        } else {
            const ptrdiff_t len = node->token->end - node->token->beg;
            printf(GREEN("%.*s "), (int) len, node->token->beg);
        }
    }

    puts("");
}
#endif

static void destroy_node(const struct node *const node)
{
    if (node->nchildren) {
        for (size_t child_idx = 0; child_idx < node->nchildren; ++child_idx) {
            destroy_node(node->children[child_idx]);
        }

        free(node->children[0]);
        free(node->children);
    }
}

static void deallocate_stack(void)
{
    free(stack.nodes);
    stack.nodes = NULL;
    stack.size = 0;
    stack.allocated = 0;
}

static void destroy_stack(void)
{
    for (size_t node_idx = 0; node_idx < stack.size; ++node_idx) {
        destroy_node(&stack.nodes[node_idx]);
    }

    deallocate_stack();
}

static inline int term_eq_node(const struct term *const term, const struct node *const node)
{
    const int node_is_leaf = node->nchildren == 0;

    if (term->is_tk == node_is_leaf) {
        if (node_is_leaf) {
            return term->tk == node->token->tk;
        } else {
            return term->nt == node->nt;
        }
    }

    return 0;
}

static size_t match_rule(const struct rule *const rule, size_t *const at)
{
    const struct term *prev = NULL;
    const struct term *term = &rule->rhs[RULE_RHS_LAST];
    ssize_t st_idx = stack.size - 1;

    do {
        if (term_eq_node(term, &stack.nodes[st_idx])) {
            prev = term->is_mt ? term : NULL;

            if (!prev) {
                --term;
            }

            --st_idx;
        } else if (prev && term_eq_node(prev, &stack.nodes[st_idx])) {
            --st_idx;
        } else if (term->is_mt) {
            prev = NULL;
            --term;
        } else {
            term = NULL;
            break;
        }
    } while (st_idx >= 0 && !(term->is_tk && term->tk == TK_COUNT));

    const int reached_eor = term && term->is_tk && term->tk == TK_COUNT;
    const size_t reduction_size = stack.size - st_idx - 1;

    return reached_eor && reduction_size ? (*at = st_idx + 1, reduction_size) : 0;
}

static int shift(const struct token *const token)
{
    if (stack.size >= stack.allocated) {
        stack.allocated = (stack.allocated ?: 1) * 8;
        struct node *const tmp = realloc(stack.nodes, stack.allocated * sizeof(struct node));

        if (!tmp) {
            return PARSE_NOMEM;
        }

        stack.nodes = tmp;
    }

    stack.nodes[stack.size++] = (struct node) {
        .nchildren = 0,
        .token = token,
    };

    return PARSE_OK;
}

static int reduce(const struct rule *const rule, const size_t at, const size_t size)
{
    struct node *const child_nodes = malloc(size * sizeof(struct node));

    if (!child_nodes) {
        return PARSE_NOMEM;
    }

    struct node *const reduce_at = &stack.nodes[at];
    struct node **const old_children = reduce_at->children;
    reduce_at->children = malloc(size * sizeof(struct node *)) ?: old_children;

    if (reduce_at->children == old_children) {
        free(child_nodes);
        return PARSE_NOMEM;
    }

    for (size_t child_idx = 0, st_idx = at; st_idx < stack.size; ++st_idx, ++child_idx) {
        child_nodes[child_idx] = stack.nodes[st_idx];
        reduce_at->children[child_idx] = &child_nodes[child_idx];
    }

    child_nodes[0].children = old_children;
    reduce_at->nchildren = size;
    reduce_at->nt = rule->lhs;
    stack.size = at + 1;
    return PARSE_OK;
}

static struct node parse(const struct token *const tokens, const size_t ntokens)
{
    for (size_t token_idx = 0; token_idx < ntokens; ) {
        for (; skip_token(tokens[token_idx].tk); ++token_idx);

        if (shift(&tokens[token_idx++])) {
            goto fail_nomem;
        }

        #ifndef NDEBUG
        printf(B_CYAN("Shift: ")), print_stack();
        #endif

        try_reduce_again:;
        for (; token_idx < ntokens && skip_token(tokens[token_idx].tk); ++token_idx);

        const struct rule *rule = grammar;

        do {
            size_t reduction_at, reduction_size;

            if ((reduction_size = match_rule(rule, &reduction_at))) {
                if (token_idx < ntokens && should_shift(rule->lhs, tokens[token_idx].tk)) {
                    if (shift(&tokens[token_idx++])) {
                        goto fail_nomem;
                    }

                    #ifndef NDEBUG
                    printf(B_CYAN("Shift: ")), print_stack();
                    #endif
                } else if (reduce(rule, reduction_at, reduction_size)) {
                    goto fail_nomem;
                } else {
                    #ifndef NDEBUG
                    const ptrdiff_t rule_number = rule - grammar + 1;
                    printf(B_BLUE("Red%02td: "), rule_number), print_stack();
                    #endif
                }

                goto try_reduce_again;
            }
        } while (++rule != grammar + GRAMMAR_SIZE);
    }

    const bool accepted = stack.size == 1 && stack.nodes[0].nchildren && stack.nodes[0].nt == NT_Dtop;
    #ifndef NDEBUG
    printf(accepted ? B_GREEN("ACCEPT ") : B_RED("REJECT ")), print_stack(), puts("");
    #endif

    struct node root;

    if (accepted) {
        root = stack.nodes[0];
        deallocate_stack();
    } else {
        destroy_stack();
        static const struct token reject = { .tk = PARSE_REJECT };
        root = (struct node) { .nchildren = 0, .token = &reject };
    }

    return root;

fail_nomem:
    destroy_stack();
    static const struct token nomem = { .tk = PARSE_NOMEM };
    return (struct node) { .nchildren = 0, .token = &nomem  };
}

static bool check_full(const struct node *const full, const bool top, const bool in_param);

static bool check_squl(const struct node *const squl, const bool top, const bool in_param)
{
    bool has_squa[TK_TLON] = { false };
    #define has(_sq) has_squa[TK_##_sq]
    bool has_storage_class = false;
    int num_long = 0;

    for (size_t i = 0; i < squl->nchildren; ++i) {
        const struct node *const squa = squl->children[i]->children[0];
        const ptrdiff_t len = squa->token->end - squa->token->beg;
        const tk_t token = squa->token->tk;

        switch (token) {
        case TK_AUTO ... TK_TYPE:
            if (!top) {
                fputs("Storage class specifier cannot appear in pointer qualification\n", stderr);
                return false;
            }

            if (in_param) {
                fputs("Storage class specifier cannot appear in function parameter\n", stderr);
                return false;
            }

            if (has_storage_class) {
                fprintf(stderr, "Storage class specifier '%.*s' is in conflict "
                    "with previous storage class specifier\n", (int) len, squa->token->beg);

                return false;
            }

            has_storage_class = true;
            break;

        case TK_CONS:
        case TK_VOLA:
        case TK_REST:
        case TK_ATOM:
            if (has_squa[token]) {
                fprintf(stderr, "Qualifier '%.*s' duplicated\n", (int) len, squa->token->beg);
                return false;
            }

            has_squa[token] = true;
            break;

        case TK_STRU ... TK_TSHR:
            if (!top) {
                fprintf(stderr, "Specifier '%.*s' cannot appear in pointer qualification\n", (int) len, squa->token->beg);
                return false;
            }

            if (has_squa[token]) {
                fprintf(stderr, "Specifier '%.*s' duplicated\n", (int) len, squa->token->beg);
                return false;
            }

            has_squa[token] = true;
            break;

        case TK_TLON:
            if (!top) {
                fputs("Specifier 'long' cannot appear in pointer qualification\n", stderr);
                return false;
            }

            num_long += 1;

            if (num_long > 2) {
                fputs("Specifier 'long' cannot appear more than twice\n", stderr);
                return false;
            }
            break;

        default:
            assert(false);
            __builtin_unreachable();
        }
    }

    if (has(STRU)) {
        if (has(ENUM) || has(VOID) || has(TCHR) || has(TINT) || has(TFLT) || has(TDBL) ||
            has(TSGN) || has(TUNS) || has(TSHR) || num_long) {

            fputs("Specifier 'struct' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(ENUM)) {
        if (has(STRU) || has(VOID) || has(TCHR) || has(TINT) || has(TFLT) || has(TDBL) ||
            has(TSGN) || has(TUNS) || has(TSHR) || num_long) {

            fputs("Specifier 'enum' is in conflict with other specifiers\n", stderr);
            return false;                
        }
    }

    if (has(VOID)) {
        if (has(STRU) || has(ENUM) || has(TCHR) || has(TINT) || has(TFLT) || has(TDBL) ||
            has(TSGN) || has(TUNS) || has(TSHR) || num_long) {

            fputs("Specifier 'void' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(TCHR)) {
        if (has(TINT) || has(TFLT) || has(TDBL) || has(TSHR) || num_long) {
            fputs("Specifier 'char' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(TINT)) {
        if (has(TFLT) || has(TDBL)) {
            fputs("Specifier 'int' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(TFLT)) {
        if (has(TDBL) || has(TSGN) || has(TUNS) || has(TSHR) || num_long) {
            fputs("Specifier 'float' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(TDBL)) {
        if (has(TFLT) || has(TSGN) || has(TUNS) || has(TSHR)) {
            fputs("Specifier 'double' is in conflict with other specifiers\n", stderr);
            return false;
        }
    }

    if (has(TSGN) && has(TUNS)) {
        fputs("Specifier 'signed' is in conflict with 'unsigned'\n", stderr);
        return false;
    }

    if (has(TSHR) && num_long) {
        fputs("Specifier 'short' is in conflict with 'long'\n", stderr);
        return false;
    }

    #undef has
    return true;
}

static bool check_full_or_list(const struct node *const full_or_list)
{
    if (full_or_list->nt == NT_Full) {
        return check_full(full_or_list, true, true);
    }

    const struct node *const left = full_or_list->children[0];
    const struct node *const right = full_or_list->children[2];

    if (left->nt == NT_List && right->nt == NT_List) {
        return check_full_or_list(left) && check_full_or_list(right);
    } else if (left->nt == NT_List) {
        return check_full_or_list(left) && check_full(right, true, true);
    } else {
        return check_full(left, true, true) && check_full(right, true, true);
    }
}

static bool check_decl(const struct node *const decl, const bool top, const bool in_param)
{
    const struct node *const dcl = decl->children[0];

    switch (dcl->nt) {
    case NT_Ndcl:
        return true;

    case NT_Adcl:
        return check_decl(dcl->children[0], top, in_param);

    case NT_Bdcl:
        if (top && !in_param) {
            fputs("Abstract array declarator needs a name\n", stderr);
            return false;
        }

        return true;

    case NT_Fdcl:
        return check_decl(dcl->children[0], top, in_param) && check_full_or_list(dcl->children[2]);

    case NT_Gdcl:
        return check_decl(dcl->children[1], top, in_param);

    case NT_Hdcl:
        if (top && !in_param) {
            fputs("Abstract function declarator needs a name\n", stderr);
            return false;
        }

        return check_full_or_list(dcl->children[1]);

    case NT_Pdcl:
        return check_decl(dcl->children[1], top, in_param);

    case NT_Qdcl:
        return check_squl(dcl->children[1], false, in_param) && check_decl(dcl->children[2], top, in_param);

    case NT_Rdcl:
        if (top && !in_param) {
            fputs("Abstract qualified pointer declarator needs a name\n", stderr);
            return false;
        }

        return check_squl(dcl->children[1], false, in_param);

    case NT_Sdcl:
        if (top && !in_param) {
            fputs("Abstract pointer declarator needs a name\n", stderr);
            return false;
        }

        return true;

    default:
        assert(false);
        __builtin_unreachable();
    }
}

static bool check_full(const struct node *const full, const bool top, const bool in_param)
{
    if (full->nchildren == 1) {
        const bool squl_ok = check_squl(full->children[0], top, in_param);

        if (top && !in_param) {
            fputs("Declaration has no declarator\n", stderr);
            return false;
        }

        return squl_ok;
    }

    return check_squl(full->children[0], top, in_param) && check_decl(full->children[1], top, in_param);
}

static bool check(const struct node *const root)
{
    return check_full(root->children[1], true, false);
}

static void print_full(const struct node *const full);

static void print_squl_sorted(const struct node *const squl, const bool want_trailing_space)
{
    bool print_started = false;

    #define PUT_SPACE ({ \
        if (print_started) { \
            putchar(' '); \
        } \
        \
        print_started = true; \
    })

    for (int round = 1; round <= 10; ++round) {
        for (size_t i = 0; i < squl->nchildren; ++i) {
            const struct node *const squa = squl->children[i]->children[0];
            const ptrdiff_t len = squa->token->end - squa->token->beg;
            const tk_t token = squa->token->tk;

            switch (token) {
            case TK_AUTO ... TK_TYPE:
                if (round == 1) {
                    PUT_SPACE;
                    printf(B_CYAN("%.*s"), (int) len, squa->token->beg);
                }
                break;

            case TK_CONS:
                if (round == 2) {
                    PUT_SPACE;
                    printf(B_GREEN("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_VOLA:
                if (round == 3) {
                    PUT_SPACE;
                    printf(B_GREEN("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_REST:
                if (round == 4) {
                    PUT_SPACE;
                    printf(B_GREEN("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_ATOM:
                if (round == 5) {
                    PUT_SPACE;
                    printf(B_GREEN("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_STRU:
            case TK_ENUM:
                if (round == 6) {
                    const struct node *const name = squl->children[i]->children[1];
                    const ptrdiff_t len_name = name->token->end - name->token->beg;
                    PUT_SPACE;
                    printf(B_YELLOW("%.*s %.*s"), (int) len, squa->token->beg, (int) len_name, name->token->beg);                    
                }
                break;

            case TK_VOID:
                if (round == 7) {
                    PUT_SPACE;
                    printf(B_YELLOW("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_TSGN:
            case TK_TUNS:
                if (round == 8) {
                    PUT_SPACE;
                    printf(B_YELLOW("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_TSHR:
            case TK_TLON:
                if (round == 9) {
                    PUT_SPACE;
                    printf(B_YELLOW("%.*s"), (int) len, squa->token->beg);                    
                }
                break;

            case TK_TCHR:
            case TK_TINT:
            case TK_TFLT:
            case TK_TDBL:
                if (round == 10) {
                    PUT_SPACE;
                    printf(B_YELLOW("%.*s"), (int) len, squa->token->beg);                    
                }
                break;
            }
        }
    }

    #undef PUT_SPACE

    if (want_trailing_space) {
        putchar(' ');
    }
}

static void print_full_or_list(const struct node *const full_or_list)
{
    if (full_or_list->nt == NT_Full) {
        print_full(full_or_list);
        return;
    }

    const struct node *const left = full_or_list->children[0];
    const struct node *const right = full_or_list->children[2];

    if (left->nt == NT_List && right->nt == NT_List) {
        print_full_or_list(left);
        printf(", ");
        print_full_or_list(right);
    } else if (left->nt == NT_List) {
        print_full_or_list(left);
        printf(", ");
        print_full(right);
    } else {
        print_full(left);
        printf(", ");
        print_full(right);
    }
}

static void print_decl(const struct node *const decl, const bool mirror_use)
{
    const struct node *const dcl = decl->children[0];

    switch (dcl->nt) {
    case NT_Ndcl: {
        const struct node *const name = dcl->children[0];
        const ptrdiff_t len = name->token->end - name->token->beg;
        printf(S_WHITE("%.*s"), (int) len, name->token->beg);
    } break;

    case NT_Adcl: {
        const struct node *const size = dcl->children[2];
        const ptrdiff_t len = size->token->end - size->token->beg;
        print_decl(dcl->children[0], mirror_use);
        printf(B_PURPLE("["));

        if (mirror_use) {
            printf(CYAN("0"));
        } else {
            printf(CYAN("%.*s"), (int) len, size->token->beg);
        }

        printf(B_PURPLE("]"));
    } break;

    case NT_Bdcl: {
        const struct node *const size = dcl->children[1];
        const ptrdiff_t len = size->token->end - size->token->beg;
        printf(B_PURPLE("[") CYAN("%.*s") B_PURPLE("]"), (int) len, size->token->beg);        
    } break;

    case NT_Fdcl:
        print_decl(dcl->children[0], mirror_use);
        printf(B_WHITE("("));

        if (mirror_use) {
            printf("...");
        } else {
            print_full_or_list(dcl->children[2]);
        }

        printf(B_WHITE(")"));
        break;

    case NT_Gdcl:
        printf(B_WHITE("(")), print_decl(dcl->children[1], mirror_use), printf(B_WHITE(")"));
        break;

    case NT_Hdcl:
        printf(B_WHITE("(")), print_full_or_list(dcl->children[1]), printf(B_WHITE(")"));
        break;

    case NT_Pdcl:
        printf(B_PURPLE("*")), print_decl(dcl->children[1], mirror_use);
        break;

    case NT_Qdcl:
        printf(B_PURPLE("*"));

        if (!mirror_use) {
            print_squl_sorted(dcl->children[1], true);
        }

        print_decl(dcl->children[2], mirror_use);
        break;

    case NT_Rdcl:
        printf(B_PURPLE("*")), print_squl_sorted(dcl->children[1], false);
        break;

    case NT_Sdcl:
        printf(B_PURPLE("*"));
        break;

    default:
        assert(false);
        __builtin_unreachable();
    }
}

static void print_full(const struct node *const full)
{
    if (full->nchildren == 1) {
        print_squl_sorted(full->children[0], false);
    } else {
        print_squl_sorted(full->children[0], true);
        print_decl(full->children[1], false);
    }
}

static void print(const struct node *const root)
{
    print_full(root->children[1]);
    puts("");
}

static void dump_array_as_boxes(const size_t size)
{
    const size_t max_boxes = 8;
    const bool do_trunc = size > max_boxes;
    const size_t num_boxes = do_trunc ? max_boxes : size;

    for (size_t i = 0; i < num_boxes; ++i) {
        printf("┌─────┐");
    }

    putchar('\n');

    for (size_t i = 0; i < num_boxes; ++i) {
        if (do_trunc && i == num_boxes - 2) {
            printf("│ ... │");
        } else if (do_trunc && i == num_boxes - 1) {
            const size_t num = size - 1;

            if (num < 10) {
                printf("│  %zu  │", num);
            } else if (num < 100) {
                printf("│ %zu  │", num);
            } else if (num < 1000) {
                printf("│ %zu │", num);
            } else if (num < 10000) {
                printf("│ %zu│", num);
            } else {
                printf("│ %zu ", num);
            }
        } else {
            printf("│  %zu  │", i);            
        }
    }

    printf(" ARR");
    putchar('\n');

    for (size_t i = 0; i < num_boxes; ++i) {
        printf("└─────┘");
    }

    putchar('\n');
}

static void dump_decl(const struct node *const decl)
{
    const struct node *const dcl = decl->children[0];

    switch (dcl->nt) {
    case NT_Ndcl: {
        print_decl(decl, true);
        putchar('\n');
    } break;

    case NT_Adcl: {
        const struct node *const size = dcl->children[2];
        const char old_char = *size->token->end;
        *((char *) size->token->end) = '\0';
        const size_t size_num = strtoull(size->token->beg, NULL, 10);
        *((char *) size->token->end) = old_char;
        dump_decl(dcl->children[0]);
        dump_array_as_boxes(size_num);
        print_decl(decl, true);
        putchar('\n');
    } break;

    case NT_Bdcl:
        break;

    case NT_Fdcl:
        dump_decl(dcl->children[0]);
        printf("┌─────┐\n");
        printf("│  𝑓  │ FN\n");
        printf("└─────┘\n");
        print_decl(decl, true);
        putchar('\n');
        break;

    case NT_Gdcl:
        dump_decl(dcl->children[1]);
        break;

    case NT_Hdcl:
        break;

    case NT_Pdcl:
        dump_decl(dcl->children[1]);
        printf("┌─────┐\n");
        printf("│  ↓  │ PTR\n");
        printf("└─────┘\n");
        print_decl(decl, true);
        putchar('\n');
        break;

    case NT_Qdcl:
        dump_decl(dcl->children[2]);
        printf("┌─────┐\n");
        printf("│  ↓  │ PTR\n");
        printf("└─────┘\n");
        print_decl(decl, true);
        putchar('\n');
        break;

    case NT_Rdcl:
    case NT_Sdcl:
        break;

    default:
        assert(false);
        __builtin_unreachable();
    }
}

static void dump_full(const struct node *const full)
{
    dump_decl(full->children[1]);
    printf(B_WHITE("↳ "));
    print_squl_sorted(full->children[0], false);
}

static void dump(const struct node *const root)
{
    dump_full(root->children[1]);
    puts("");
}

int main(const int argc, const char *const *const argv)
{
    int exit_status = EXIT_FAILURE;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cdecl>\n", argv[0]);
        goto fail;
    }

    const char *const cdecl = argv[1];

    if (!*cdecl) {
        fputs("Empty argument\n", stderr);
        goto fail;
    }

    struct token *tokens;
    size_t ntokens;

    switch (lex(cdecl, &tokens, &ntokens)) {
    case LEX_BAD:
        print_lex_error(tokens, ntokens);
        goto fail_free_tokens;

    case LEX_NOMEM:
        fputs("Out of memory while lexing\n", stderr);
        goto fail_free_tokens;
    }

    const struct node root = parse(tokens, ntokens);

    switch (parse_error(root)) {
    case PARSE_REJECT:
        fputs("Parse error\n", stderr);
        goto fail_free_tokens;

    case PARSE_NOMEM:
        fputs("Out of memory while parsing\n", stderr);
        goto fail_free_tokens;
    }

    if (!check(&root)) {
        goto fail_destroy_tree;
    }

    print(&root);
    puts("");
    dump(&root);
    exit_status = EXIT_SUCCESS;

fail_destroy_tree:
    destroy_node(&root);
fail_free_tokens:
    free(tokens);
fail:
	return exit_status;
}
