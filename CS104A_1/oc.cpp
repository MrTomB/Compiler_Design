//Thomas Burch
//tburch1@ucsc.edu
//CMPS104A-ASG1

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxlib.h"

//ADDED
#include <unistd.h>
#include "stringset.h"

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;

int yydebug = 0;
int yy_flex_debug = 0;
string command = CPP;
char *finalName;
FILE *finalFile; 

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

void getopt (int argc, char** argv){
    int opt;
    if(argc == 1){
        fprintf(stderr, "Usage: %s [ -ly ] [ -@ flag ...] [ -D string] "
            "program.oc\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    while((opt = getopt(argc, argv, "ly@:D:")) != -1){
        switch(opt){
            case 'l':
                printf("yy_flex true\n");
                yy_flex_debug = 1;
                break;
            case 'y':
                printf("yydebug true\n");
                yydebug = 1;
                break;
            case '@':
                set_debugflags(optarg);
                /* use __debugprintf (char flag, const char* 
                *  file, int line,const char* func, const 
                *char* format, ...) to print out debug statements
                */
                break;
            case 'D':
                command += " -D";
                command += optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [ -ly ] [ -@ flag ...] "
                  "[ -D string] program.oc\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc){
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    return;
} 


int main (int argc, char** argv) {
   set_execname (argv[0]);
   getopt(argc, argv); // setting options
   char* filename;

   for (int argi = 1; argi < argc; ++argi) {
      filename = argv[argi];
      command = command + " " + filename;
      //printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         syserrprintf (command.c_str());
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
      }
   }

   set_execname (filename);
   finalName = basename(filename);
   finalName = strtok(finalName, ".");
   strcat(finalName, ".str");
   finalFile = fopen(finalName, "w");
   dump_stringset (finalFile);

   return get_exitstatus();
}
