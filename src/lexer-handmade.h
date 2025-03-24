#pragma once

#include "cutie.h"
#define TOKEN_LIST                \
  __PUNC(PLUS,       "+" )        \
  __PUNC(STAR,       "*" )        \
  __PUNC(SEMICOLON,  ";" )        \
  __PUNC(OPAREN,     "(" )        \
  __PUNC(CPAREN,     ")" )        \
  __PUNC(OCURLY,     "{" )        \
  __PUNC(CCURLY,     "}" )        \
  __PUNC(GT,         ">" )        \
  __PUNC(GTEQUAL,    ">=")        \
  __PUNC(EQUAL,      "=" )        \
  __PUNC(DEQUAL,     "==")        \
                                  \
  __KEYWORD(BOOL,        "bool"  )   \
  __KEYWORD(INT,         "int"   )   \
  __KEYWORD(IF,          "if"    )   \
  __KEYWORD(THEN,        "then"  )   \
  __KEYWORD(ELSE,        "else"  )   \
  __KEYWORD(DO,          "do"    )   \
  __KEYWORD(WHILE,       "while" )   \
  __KEYWORD(PRINT,       "print" )   \
  __KEYWORD(BEGIN,       "begin" )   \
  __KEYWORD(END,         "end"   )   \

enum {
#define __PUNC(n, s) TOK_##n,
#define __KEYWORD(n, s) TOK_##n,
  TOKEN_LIST
#undef __PUNC
#undef __KEYWORD
  TOK_OVER,
  TOK_IDENT,
  TOK_NUMBER,
  TOK_INVALID,
};

typedef struct lexer lexer_t;
struct lexer
{
  c8arr_t buffer;
  u32 current;

  u32 file_line;
  u32 file_col;
  FILE *file;
};

typedef struct{
  i32 token_type;
  u32 file_line;
  u32 file_col;
  union{
    struct {
      c8arr_t name;
    } identifer;
    struct {
      int value;
    } number;
  };
} token_data_t;


extern const char *g_token_name[]; 

void lexer_init(lexer_t *l, c8 *src, usize src_len);
i32 lexer_lex(lexer_t *l, token_data_t *token, arena_t *arena);
