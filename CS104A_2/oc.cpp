//Thomas Burch
//tburch1@ucsc.edu
//CMPS104A-ASG2

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxlib.h"

//ADDED ASG1
#include <unistd.h>
#include "stringset.h"
//ADDED ASG2
#include "yylex.cc"
#include "astree.h"
#include "lyutils.h"

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;

char* filename;
string command = CPP;
FILE *finalFile;
char *finalName;

//-------------------------------------------------------
// **from:/afs/cats.ucsc.edu/courses/cmps104a-wm/Examples/e08.expr-smc/
// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assignes opened pipe to FILE* yyin.

string yyin_cpp_command;

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
      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, filename);
      if (sscanf_rc == 2) {
         //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         //printf ("token %d.%d: [%s]\n",
         //        linenr, tokenct, token);
         intern_stringset(token); //ADDED call intern_stringset     
      }
      ++linenr;
   }
}
//-------------------------------------------------------

void scan_opt (int argc, char** argv){
    int opt;
    opterr = 0;
    yy_flex_debug = 0;
    yydebug = 0;
    if(argc == 1){
        fprintf(stderr, "Usage: %s [ -ly ] [ -@ flag ...] [ -D string] "
            "program.oc\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for(;;){
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
	
    if (optind >= argc){
        fprintf(stderr, "Expected argument after options\n");
        exit(get_exitstatus());
    }
    //ADDED
    const char* filename = optind == argc ? "-" : argv[optind];
    yyin_cpp_popen (filename);
} 

//ASG2 function produces .tok file
void output_tok () {
    // printf("output_tok:filename: %s\n", filename);
    //opens pipe
    yyin_cpp_popen(filename);
    //duplicate so it wont overwrite
    finalName = filename;
    // remove extenstion and add .tok
    finalName = strtok(finalName, ".");
    strcat(finalName, ".tok");
    //removes old token files (deletes)
    remove(finalName);
    //parses
    while(yylex() != YYEOF);
    //close the pipe
    yyin_cpp_pclose();
}

//ASG1 fuinction produces .str file
void output_str(){
  	finalName = filename;

   //for (int argi = 1; argi < argc; ++argi) {
   //filename = argv[argi];
   //command = command + " " + filename;
      //printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         syserrprintf (command.c_str());
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
      }
   //}
   //set_execname (filename);
   //finalName = basename(filename);
   finalName = strtok(finalName, ".");
   strcat(finalName, ".str");
   finalFile = fopen(finalName, "w");
   dump_stringset (finalFile);
}

int main (int argc, char** argv) {
   set_execname (argv[0]);
   scan_opt(argc, argv); // setting options

   filename = argv[argc-1];
    // __debugprintf ('t', "test", 20, "main", "%s");
    if(get_exitstatus()) return get_exitstatus();

    command += " ";
    command += filename;

    //remove path
    filename = basename(filename);

   output_tok(); //creates the .tok file

   output_str(); //creates program.str

   return get_exitstatus();
}









