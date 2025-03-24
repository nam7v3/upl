#include <stdio.h>
#include "lexer-flex.h"
#include "lexer-handmade.h"

#define CUTIE_IMPLEMENTATION
#include "cutie.h"

typedef struct {
  FILE *outfile;
  FILE *infile;
  int backend;
} options_t;
options_t g_options;

void print_usage()
{
  puts("Usage: lexer [OPTIONS] FILE");
  puts("A lexer for the UPL language");
  puts("OPTIONS:");
  puts("   -o outfile   output token to a file (default to stdout)");
  puts("   -b backend   what backend to use (\"handmade\"or \"flex\")");
  puts("   -h --help    print this help message");
  exit(0);
}

void parse_options(int argc, char *argv[])
{
  #define shift(argc, argv) argc--, argv++;
  shift(argc, argv);
  if(argc <= 0) print_usage();
  while(true)
  {
    if(strcmp(*argv, "-o") == 0) {
      shift(argc, argv);
      if(argc > 0)
      {
        g_options.outfile = fopen(*argv, "w");
        shift(argc, argv);
        continue;
      }
      else {
        puts("Missing output file.");
        exit(EXIT_FAILURE);
      }
    }
    if(strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0) {
      print_usage();
    }
    if(strcmp(*argv, "-b") == 0)
    {
      shift(argc, argv);
      if(argc > 0)
      {
        if(strcmp(*argv, "handmade") == 0) {
          shift(argc, argv);
          g_options.backend = 1;
          continue;
        }
        if(strcmp(*argv, "flex") == 0) {
          shift(argc, argv);
          g_options.backend = 0;
          continue;
        }

        printf("Invalid backend %s, handmade or flex.\n", *argv);
        exit(EXIT_FAILURE);
      }
      else
      {
        puts("Missing backend. handmade or flex.");
        exit(EXIT_FAILURE);
      }
    }
    break;
  }
  if(argc > 1) {
    puts("Only allowed one input file.");
    exit(EXIT_FAILURE);
  }
  if(argc <= 0) {
    puts("Missing input file.");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(*argv, "r");
  if(!file) {
    perror("");
    exit(EXIT_FAILURE);
  }
  g_options.infile = file;
}  

int main(int argc, char *argv[])
{
  g_options.outfile = stdout;
  g_options.backend = 1;
  parse_options(argc, argv);
  
  lexer_t lexer;

  if(g_options.backend){
    arena_t allocator = makearena(10 * 1024 * 1024);
    i32 token;
    u8arr_t file = areadallfile(&allocator, "./test.upl");
    lexer_init(&lexer, (c8*) file.data, file.len);
    do {
      token_data_t token_data;
      token = lexer_lex(&lexer, &token_data, &allocator);
      fprintf(g_options.outfile, "%s(line=%d, col=%d", g_token_name[token], token_data.file_line, token_data.file_col);
      switch(token_data.token_type)
      {
        case TOK_IDENT:
          fprintf(g_options.outfile, ",name = \"%.*s\"", token_data.identifer.name.len, token_data.identifer.name.data);
          break;
        case TOK_NUMBER:
          fprintf(g_options.outfile, ", %d", token_data.number.value);
          break;
      }
      fprintf(g_options.outfile, ")\n");
    
    } while (token != TOK_OVER);
  } else {
    yyin = g_options.infile;
    yyout = g_options.outfile;
    yylex();
  }
  return 0;
}
