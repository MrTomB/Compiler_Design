/* Thomas Burch (1314305)
    tburch1@ucsc.edu
    CMPS104A-ASG3
*/

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"


astree::astree (int symbol, int filenr, int linenr,
                int offset, const char* clexinfo):
        symbol (symbol), filenr (filenr), linenr (linenr),
        offset (offset), lexinfo (intern_stringset (clexinfo)) {
   DEBUGF ('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
           (void*) this, filenr, linenr, offset,
           get_yytname (symbol), lexinfo->c_str());
}

astree* adopt1 (astree* root, astree* child) {
   root->children.push_back (child);
   DEBUGF ('a', "%p (%s) adopting %p (%s)\n",
           root, root->lexinfo->c_str(),
           child, child->lexinfo->c_str());
   return root;
}

astree* adopt2 (astree* root, astree* left, astree* right) {
   adopt1 (root, left);
   adopt1 (root, right);
   return root;
}

//ASG3
astree* adopt1sym (astree* root, astree* child, int symbol) {
   root = adopt1 (root, child);
   root->symbol = symbol;
   return root;
}

// ASG3
void change_symbol(astree* child, int symbol) {
    child->symbol = symbol;
}

// ASG3
static void dump_node_tok(FILE* outfile, astree* node) {
    if( node->symbol == TOK_BAD_IDENT ||
       node->symbol == TOK_BAD_STRING ||
       node->symbol == TOK_BAD_CHAR){
        errprintf("Invalid token on line: %d\n", node->linenr);
    }
    fprintf (outfile, "%4d  %4ld.%03ld  %3d  %-13s  (%s)\n",
             (int)node->filenr, node->linenr, node->offset,
             node->symbol, get_yytname(node->symbol),
             node->lexinfo->c_str());
}


//ASG3 Changed from ASG2
static void dump_node (FILE* outfile, astree* node) {
  fprintf(outfile, "%s \"%s\" %ld.%ld.%ld", get_yytname(node->symbol),
      node->lexinfo->c_str(), node->filenr, node->linenr, node->offset);
}

/* 
  fprintf (outfile, "%p->{%s(%d) %ld:%ld.%03ld \"%s\" [",
            node, get_yytname (node->symbol), node->symbol,
            node->filenr, node->linenr, node->offset,
            node->lexinfo->c_str());

  // ADDED
  if (node == NULL) return;
   fprintf (outfile, "%4d%4ld.%03ld %4d %-16s (%s)\n",
           (int)node->filenr, node->linenr, node->offset,
           node->symbol, get_yytname(node->symbol),
           node->lexinfo->c_str());

   bool need_space = false;
   for (size_t child = 0; child < node->children.size();
        ++child) {
      if (need_space) fprintf (outfile, " ");
      need_space = true;
      fprintf (outfile, "%p", node->children.at(child));
   }
   // fprintf (outfile, "]}");
}*/

static void dump_astree_rec (FILE* outfile, astree* root, int depth) {
   
   if (root == NULL) return;
   //fprintf (outfile, "%*s%s ", depth * 3, "",
   //         root->lexinfo->c_str());
   fprintf (outfile, "%*s ", depth * 3, ""); 
   dump_node (outfile, root);

   fprintf (outfile, "\n");
   for (size_t child = 0; child < root->children.size();
        ++child) {
      dump_astree_rec (outfile, root->children[child],
                       depth + 1);
   }
}

void dump_astree (FILE* outfile, astree* root) {
   dump_astree_rec (outfile, root, 0);
   fflush (NULL);
}

//ASG3
void dump_astree_not_rec (FILE* outfile, astree* root){
    if ( root == NULL) return;
    dump_node_tok(outfile, root);
}

void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep) {
  DEBUGF ('f', "toknum = %d, yyvaluep = %p\n", toknum, yyvaluep);
   if (is_defined_token (toknum)) {
      dump_node (outfile, yyvaluep);
   }else {
      fprintf (outfile, "%s(%d)\n",
               get_yytname (toknum), toknum);
   }
   fflush (NULL);
}


void free_ast (astree* root) {
   while (not root->children.empty()) {
      astree* child = root->children.back();
      root->children.pop_back();
      free_ast (child);
   }
   DEBUGF ('f', "free [%p]-> %d:%d.%d: %s: \"%s\")\n",
           root, root->filenr, root->linenr, root->offset,
           get_yytname (root->symbol), root->lexinfo->c_str());
   delete root;
}

void free_ast2 (astree* tree1, astree* tree2) {
   free_ast (tree1);
   free_ast (tree2);
}

RCSC("$Id: astree.cpp,v 1.6 2015-04-09 19:31:47-07 - - $")

