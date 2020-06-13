/* Thomas Burch (1314305)
    tburch1@ucsc.edu
    CMPS104A-ASG3
*/

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

//for getopt
#include <unistd.h>
//ASG1
#include "auxlib.h"
#include "stringset.h"
//ASG2
#include "yylex.cc"
#include "astree.h"
#include "lyutils.h"

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;

char *filename;

string command = CPP;
string yyin_cpp_command;

//-------------------------------------------------------
// **from:/afs/cats.ucsc.edu/courses/cmps104a-wm/Examples/e08.expr-smc/
// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assignes opened pipe to FILE* yyin.
void yyin_cpp_popen (const char* filename) {
   yyin_cpp_command = CPP;
   yyin_cpp_command += " ";
   yyin_cpp_command += filename;
   yyin = popen (yyin_cpp_command.c_str(), "r");
   if (yyin == NULL) {
      syserrprintf (yyin_cpp_command.c_str());
      exit (get_exitstatus());
   }
}

void yyin_cpp_pclose (void) {
   int pclose_rc = pclose (yyin);
   eprint_status (yyin_cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) set_exitstatus (EXIT_FAILURE);
}

//-------------------------------------------------------
// $Id: cppstrtok.cpp,v 1.3 2014-10-07 18:09:11-07 - - $
// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy (inputname, filename);
    for (;;) {
        char buffer[LINESIZE];
        char* fgets_rc = fgets (buffer, LINESIZE, pipe);
        if (fgets_rc == NULL) break;
        chomp (buffer, '\n');
        // printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
        // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
        int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                                &linenr, filename);
        if (sscanf_rc == 2) {
            //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, 
            // filename);
            continue;
        }
        char* savepos = NULL;
        char* bufptr = buffer;
        for (int tokenct = 1;; ++tokenct) {
            char* token = strtok_r (bufptr, " \t\n", &savepos);
            bufptr = NULL;
            if (token == NULL) break;
            //printf ("token %d.%d: [%s]\n", linenr, tokenct, token);
            // added following line
            intern_stringset (token); 
        }
        ++linenr;
   }
}

//-------------------------------------------------------

void scan_opts (int argc, char** argv) {
    int opt;
    opterr = 0;
    yy_flex_debug = 0;
    yydebug = 0;
    if(argc == 1) {
        fprintf(stderr, "Usage: %s [ -ly ] [ -@ flag ...] [ -D string] "
            "program.oc\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    for(;;) {
        opt = getopt(argc, argv, "ly@:D:");
        if(opt == EOF) break;
        switch(opt) {
            case 'l': yy_flex_debug = 1; break;
            case 'y': yydebug = 1; break;
            /* use __debugprintf (char flag, const char* 
             *  file, int line,const char* func, const 
             *  char* format, ...) to print out debug statements
             */
            case '@':set_debugflags(optarg); break;
            case 'D': command += " -D"; command += optarg; break;
            default: errprintf ("%:bad option (%c)\n", optopt); break;
        }
    }

    if (optind > argc) {
        errprintf ("Usage: %s [ -ly ] [ -@ flag ...] "
                  "[ -D string] program.oc\n", get_execname());
        exit(get_exitstatus());
    }
    //ADDED
    const char* filename = optind == argc ? "-" : argv[optind];
    yyin_cpp_popen (filename);
} 

//ASG3 function produces .sym file
void output_sym () {
    //opens pipe
    char *final_sym_name = filename;
    // remove extenstion and add .ast
    final_sym_name = strtok(final_sym_name, ".");
    strcat(final_ast_name, ".sym");
    //removes old ast files (deletes)
    remove(final_ast_name);
    // opens file with correct name and extension
    FILE *final_sym_file = fopen(final_sym_name, "w");
    // dumps the abstract syntax tree to file
    dump_astree(final_sym_file, yyparse_astree);
    //close file
    fclose(final_sym_file);
}



//ASG3 function produces .ast file
void output_ast () {
    //opens pipe
    char *final_ast_name = filename;
    // remove extenstion and add .ast
    final_ast_name = strtok(final_ast_name, ".");
    strcat(final_ast_name, ".ast");
    //removes old ast files (deletes)
    remove(final_ast_name);
    // opens file with correct name and extension
    FILE *final_ast_file = fopen(final_ast_name, "w");
    // dumps the abstract syntax tree to file
    dump_astree(final_ast_file, yyparse_astree);
    //close file
    fclose(final_ast_file);
}


//ASG2 function produces .tok file
void output_tok () {
    //opens pipe
    yyin_cpp_popen(filename);
    //duplicate so it wont overwrite
    final_tok_name = filename;
    // remove extenstion and add .tok
    final_tok_name = strtok(final_tok_name, ".");
    strcat(final_tok_name, ".tok");
    //removes old token files (deletes)
    remove(final_tok_name);
    //parses
    //while(yylex() != YYEOF);
//ADDED ASG3:    
    yyparse();
    //close the pipe
    yyin_cpp_pclose();
}

//ASG1 fuinction produces .str file
void output_str () {
    char* final_str_name = filename;
    FILE *final_str_file;

    FILE* pipe = popen (command.c_str(), "r");
    if (pipe == NULL) {
       syserrprintf (command.c_str());
    }else {
       cpplines (pipe, filename);
       int pclose_rc = pclose (pipe);
       eprint_status (command.c_str(), pclose_rc);
    }

    //remove .oc extension
    final_str_name = strtok(final_str_name, ".");
    strcat(final_str_name, ".str");
    final_str_file = fopen(final_str_name, "w");
    dump_stringset (final_str_file);
}

int main (int argc, char** argv) {
    set_execname (argv[0]);
    scan_opts(argc, argv);
    filename = argv[argc-1];

    if(get_exitstatus()) return get_exitstatus();

    command = command + " " + filename;
    
    filename = basename(filename); //remove path

    output_tok(); //creates the .tok file
    
    output_str(); //creates the .str file

    output_ast(); //creates the .ast file

    output_sym(); //creates the .sym file
    
    return get_exitstatus();
}
