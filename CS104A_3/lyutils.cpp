/* Thomas Burch (1314305)
    tburch1@ucsc.edu
    CMPS104A-ASG3
*/

#include <vector>
#include <string>
using namespace std;

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lyutils.h"
#include "auxlib.h"

astree* yyparse_astree = NULL;
int scan_linenr = 1;
int scan_offset = 0;
bool scan_echo = false;
vector<string> included_filenames;

//ADDED
FILE* final_tok_file = NULL;
char *final_tok_name;

const string* lexer_filename (int filenr) {
   return &included_filenames.at(filenr);
}

void lexer_newfilename (const char* filename) {
   included_filenames.push_back (filename);
}

void lexer_newline (void) {
   ++scan_linenr;
   scan_offset = 0;
}

void lexer_setecho (bool echoflag) {
   scan_echo = echoflag;
}


void lexer_useraction (void) {
   if (scan_echo) {
      if (scan_offset == 0) printf (";%5d: ", scan_linenr);
      printf ("%s", yytext);
   }
   scan_offset += yyleng;
}

void yyerror (const char* message) {
   assert (not included_filenames.empty());
   errprintf ("%:%s: %d: %s\n",
              included_filenames.back().c_str(),
              scan_linenr, message);
}

void lexer_badchar (unsigned char bad) {
   char char_rep[16];
   sprintf (char_rep, isgraph (bad) ? "%c" : "\\%03o", bad);
   errprintf ("%:%s: %d: invalid source character (%s)\n",
              included_filenames.back().c_str(),
              scan_linenr, char_rep);
}

void lexer_badtoken (char* lexeme) {
   errprintf ("%:%s: %d: invalid token (%s)\n",
              included_filenames.back().c_str(),
              scan_linenr, lexeme);
}

int yylval_token (int symbol) {
  //ADDED: prepare file for output based of program name
  final_tok_file = fopen(final_tok_name, "a");

  // make the astree
   int offset = scan_offset - yyleng;
   yylval = new astree (symbol, included_filenames.size() - 1,
                        scan_linenr, offset, yytext);
   // ADDED: dump this astree to file and close
   //dump_astree(final_tok_file, yylval);
   //ADDED ASG3:
   dump_astree_not_rec(final_tok_file, yylval);
   fclose(final_tok_file);

   return symbol;
}

astree* new_parseroot (void) {
  // yyparse_astree = new astree (TOK_ROOT, 0, 0, 0, "");
  // ADDED
   yyparse_astree = new astree (TOK_ROOT, 0, 0, 0, "<<ROOT>>");
   return yyparse_astree;
}


void lexer_include (void) {
  // ADDED: open new file from program name
   final_tok_file = fopen(final_tok_name, "a");

   lexer_newline();
   char filename[strlen (yytext) + 1];
   int linenr;
   int scan_rc = sscanf (yytext, "# %d \"%[^\"]\"",
                         &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%: %d: [%s]: invalid directive, ignored\n",
                 scan_rc, yytext);
   }else {
      fprintf (final_tok_file, "# %d \"%s\"\n", linenr, filename);
      lexer_newfilename (filename);
      scan_linenr = linenr - 1;
      DEBUGF ('m', "filename=%s, scan_linenr=%d\n",
              included_filenames.back().c_str(), scan_linenr);
   }
   // ADDED: close the token output file
   fclose(final_tok_file);
}

RCSC("$Id: lyutils.cpp,v 1.5 2015-04-09 19:34:17-07 - - $")

