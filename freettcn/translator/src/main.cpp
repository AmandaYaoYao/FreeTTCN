//
// Copyright (C) 2007 Mateusz Pusz
//
// This file is part of freettcn (Free TTCN) compiler.

// freettcn is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// freettcn is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with freettcn; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


#include "ttcn3Lexer.h"
#include "ttcn3Parser.h"
#include <iostream>
#include <iomanip>
//#include <algorithm>


namespace freettcn {

  namespace translator {

    static void DisplayRecognitionError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 *tokenNames)
    {
      pANTLR3_EXCEPTION ex = recognizer->state->exception;
      std::string fileName;
      
      // See if there is a 'filename' we can use
      if(!ex->streamName) {
        if(((pANTLR3_COMMON_TOKEN)ex->token)->type == ANTLR3_TOKEN_EOF)
          fileName = "<EOF>";
        else
          fileName = "<UNKNOWN>";
      }
      else {
        pANTLR3_STRING ftext;
        ftext = ex->streamName->to8(ex->streamName);
        fileName = (char *)ftext->chars;
      }
      
      std::cerr << fileName << ":";
      
      // Next comes the line number
      std::cerr << ex->line << ":";
      
      // How we determine the next piece is dependent on which thing raised the error.
      switch(recognizer->type) {
      case ANTLR3_TYPE_LEXER:
        {
          pANTLR3_LEXER lexer = (pANTLR3_LEXER)recognizer->super;
          
          std::cerr << ex->charPositionInLine + 1 << ":";
          std::cerr << " error: LEXER: ";
          
          if(ex->type == ANTLR3_NO_VIABLE_ALT_EXCEPTION)
            std::cerr << "Cannot match to any predicted input";
          else
            std::cerr << (pANTLR3_UINT8)ex->message;

          ANTLR3_INT32 width;
          
          width = ANTLR3_UINT32_CAST(( (pANTLR3_UINT8)lexer->input->data + (lexer->input->size(lexer->input) )) - (pANTLR3_UINT8)(ex->index));

          if(width >= 1) {
            if(isprint(ex->c))
              std::cerr << " near ‘" << (char)ex->c << "’";
            else
              std::cerr << " near char(" << std::hex << std::setw(2) << (ANTLR3_UINT8)ex->c;
            // ANTLR3_FPRINTF(stderr, "\t%.*s\n", width > 20 ? 20 : width ,((pANTLR3_UINT8)ex->index));
          }
          else {
            std::cerr << " ‘<EOF>’" << std::endl;
            std::cerr << fileName << ":" << (ANTLR3_UINT32)(lexer->rec->state->tokenStartLine) <<
              ":" << (ANTLR3_UINT32)(lexer->rec->state->tokenStartCharPositionInLine) << ": ";
            width = ANTLR3_UINT32_CAST(((pANTLR3_UINT8)lexer->input->data + (lexer->input->size(lexer->input))) - (pANTLR3_UINT8)lexer->rec->state->tokenStartCharIndex);
            if(width >= 1)
              std::cerr << "The lexer was matching from: " << std::string((char *)lexer->rec->state->tokenStartCharIndex, 0, 20) << std::endl;
            else
              std::cerr << "The lexer was matching from the end of the line" << std::endl;
            
            std::cerr << "NOTE: Above errors indicates a poorly specified lexer RULE or unterminated input element" << std::endl;
            std::cerr << "      such as: \"STRING[\"]";
          }
          std::cerr << std::endl;
        }
        break;

      case ANTLR3_TYPE_PARSER:
        {
          pANTLR3_COMMON_TOKEN token = (pANTLR3_COMMON_TOKEN)ex->token;
          
          std::cerr << ex->charPositionInLine + 1 << ":";
          std::cerr << " error: PARSER: ";
          
          if(ex->type == ANTLR3_NO_VIABLE_ALT_EXCEPTION)
            std::cerr << "Cannot match ‘" << token->getText(token)->chars << "’ to any predicted input";
          else {
            std::cerr << (pANTLR3_UINT8)ex->message;
          
            if(token) {
              if (token->type == ANTLR3_TOKEN_EOF)
                std::cerr << " at ‘<EOF>’";
              else {
                if(ex->type == ANTLR3_MISSING_TOKEN_EXCEPTION) {
                  if(!tokenNames)
                    std::cerr << " [" << ex->expecting << "]";
                  else {
                    if(ex->expecting == ANTLR3_TOKEN_EOF)
                      std::cerr << " ‘<EOF>’";
                    else
                      std::cerr << " ‘" << tokenNames[ex->expecting] << "’";
                  }
                }
                else {
                  std::cerr << " near ‘" << token->getText(token)->chars << "’";
                  // ANTLR3_FPRINTF(stderr, "\n    near %s\n    ", ttext == NULL ? (pANTLR3_UINT8)"<no text for the token>" : ttext->chars);
                }
              }
            }

            if(ex->type == ANTLR3_UNWANTED_TOKEN_EXCEPTION) {
              if(tokenNames) {
                if(ex->expecting == ANTLR3_TOKEN_EOF)
                  std::cerr << " (‘<EOF>’ expected)";
                else
                  std::cerr << " (‘" << tokenNames[ex->expecting] << "’ expected)";
              }
            }
          }
          std::cerr << std::endl;
        }
        break;
        
// case ANTLR3_TYPE_TREE_PARSER:
//           {
//           pANTLR3_TREE_PARSER tparser = (pANTLR3_TREE_PARSER) (recognizer->super);
//           pANTLR3_INT_STREAM is = tparser->ctnstream->tnstream->istream;
//           pANTLR3_BASE_TREE baseTree = (pANTLR3_BASE_TREE)(ex->token);
//           ttext = theBaseTree->toStringTree(theBaseTree);

//           if  (theBaseTree != NULL)
//             {
//               pANTLR3_COMMON_TREE commonTree = (pANTLR3_COMMON_TREE)theBaseTree->super;

//               if(theCommonTree != NULL)
//                 {
//                   theToken = (pANTLR3_COMMON_TOKEN)    theBaseTree->getToken(theBaseTree);
//                 }
//               ANTLR3_FPRINTF(stderr, ", at offset %d", theBaseTree->getCharPositionInLine(theBaseTree));
//               ANTLR3_FPRINTF(stderr, ", near %s", ttext->chars);
//             }
//           }
//           break;

      default:
        std::cerr << "DisplayRecognitionError() called by unknown parser type - provide override for this function" << std::endl;
        return;
      }
      
      switch(ex->type) {
      case ANTLR3_UNWANTED_TOKEN_EXCEPTION:
        // Indicates that the recognizer was fed a token which seesm to be
        // spurious input. We can detect this when the token that follows
        // this unwanted token would normally be part of the syntactically
        // correct stream. Then we can see that the token we are looking at
        // is just something that should not be there and throw this exception.
        //
        if(recognizer->type != ANTLR3_TYPE_PARSER) {
          if(tokenNames == NULL)
            ANTLR3_FPRINTF(stderr, " : Extraneous input...");
          else {
            if(ex->expecting == ANTLR3_TOKEN_EOF)
              ANTLR3_FPRINTF(stderr, " : Extraneous input - expected <EOF>\n");
            else
              ANTLR3_FPRINTF(stderr, " : Extraneous input - expected %s ...\n", tokenNames[ex->expecting]);
          }
        }
        break;
        
      case ANTLR3_MISSING_TOKEN_EXCEPTION:
        // Indicates that the recognizer detected that the token we just
        // hit would be valid syntactically if preceeded by a particular 
        // token. Perhaps a missing ';' at line end or a missing ',' in an
        // expression list, and such like.
        break;
        
      case ANTLR3_RECOGNITION_EXCEPTION:
        // Indicates that the recognizer received a token
        // in the input that was not predicted. This is the basic exception type 
        // from which all others are derived. So we assume it was a syntax error.
        // You may get this if there are not more tokens and more are needed
        // to complete a parse for instance.
        break;
        
      case ANTLR3_MISMATCHED_TOKEN_EXCEPTION:
        // We were expecting to see one thing and got another. This is the
        // most common error if we coudl not detect a missing or unwanted token.
        // Here you can spend your efforts to
        // derive more useful error messages based on the expected
        // token set and the last token and so on. The error following
        // bitmaps do a good job of reducing the set that we were looking
        // for down to something small. Knowing what you are parsing may be
        // able to allow you to be even more specific about an error.
        //
        if(!tokenNames)
          ANTLR3_FPRINTF(stderr, " : syntax error...\n");
        else {
          if(ex->expecting == ANTLR3_TOKEN_EOF)
            ANTLR3_FPRINTF(stderr, " : expected <EOF>\n");
          else
            ANTLR3_FPRINTF(stderr, " : expected %s ...\n", tokenNames[ex->expecting]);
        }
        break;
        
      case ANTLR3_NO_VIABLE_ALT_EXCEPTION:
        // We could not pick any alt decision from the input given
        // so god knows what happened - however when you examine your grammar,
        // you should. It means that at the point where the current token occurred
        // that the DFA indicates nowhere to go from here.
        //
        if(recognizer->type != ANTLR3_TYPE_LEXER && recognizer->type != ANTLR3_TYPE_PARSER)
          ANTLR3_FPRINTF(stderr, " : cannot match to any predicted input...\n");
        break;
        
      case ANTLR3_MISMATCHED_SET_EXCEPTION:
        {
          ANTLR3_UINT32 count;
          ANTLR3_UINT32 bit;
          ANTLR3_UINT32 size;
          ANTLR3_UINT32 numbits;
          pANTLR3_BITSET errBits;
          
          // This means we were able to deal with one of a set of
          // possible tokens at this point, but we did not see any
          // member of that set.
          //
          ANTLR3_FPRINTF(stderr, " : unexpected input...\n  expected one of : ");
          
          // What tokens could we have accepted at this point in the
          // parse?
          //
          count   = 0;
          errBits = antlr3BitsetLoad(ex->expectingSet);
          numbits = errBits->numBits(errBits);
          size    = errBits->size(errBits);
          
          if(size > 0) {
            // However many tokens we could have dealt with here, it is usually
            // not useful to print ALL of the set here. I arbitrarily chose 8
            // here, but you should do whatever makes sense for you of course.
            // No token number 0, so look for bit 1 and on.
            //
            for(bit = 1; bit < numbits && count < 8 && count < size; bit++) {
              // TODO: This doesn;t look right - should be asking if the bit is set!!
              //
              if  (tokenNames[bit]) {
                ANTLR3_FPRINTF(stderr, "%s%s", count > 0 ? ", " : "", tokenNames[bit]); 
                count++;
              }
            }
            ANTLR3_FPRINTF(stderr, "\n");
          }
          else {
            ANTLR3_FPRINTF(stderr, "Actually dude, we didn't seem to be expecting anything here, or at least\n");
            ANTLR3_FPRINTF(stderr, "I could not work out what I was expecting, like so many of us these days!\n");
          }
        }
        break;
        
      case ANTLR3_EARLY_EXIT_EXCEPTION:
        // We entered a loop requiring a number of token sequences
        // but found a token that ended that sequence earlier than
        // we should have done.
        //
        ANTLR3_FPRINTF(stderr, " : missing elements...\n");
        break;
        
      default:
        
        // We don't handle any other exceptions here, but you can
        // if you wish. If we get an exception that hits this point
        // then we are just going to report what we know about the
        // token.
        //
        ANTLR3_FPRINTF(stderr, " : syntax not recognized...\n");
        break;
      }
    }

  }  // namespace translator

}  // namespace freettcn


int main(int argc, char *argv[])
{
  pANTLR3_UINT8                  fName = (pANTLR3_UINT8)argv[1];
  pANTLR3_INPUT_STREAM           input;
  pttcn3Lexer                    lex;
  pANTLR3_COMMON_TOKEN_STREAM    tokens;
  pttcn3Parser                   parser;

  input = antlr3AsciiFileStreamNew(fName);
  if(!input)
    std::cerr << "Unable to open file " << (char *)fName << " due to malloc() failure1" << std::endl;

  lex = ttcn3LexerNew(input);
  if(!lex) {
    std::cerr << "Unable to create the lexer due to malloc() failure1" << std::endl;
    exit(ANTLR3_ERR_NOMEM);
  }

  // Override warning messages
  lex->pLexer->rec->displayRecognitionError = freettcn::translator::DisplayRecognitionError;

  tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  if(!tokens) {
    std::cerr << "Out of memory trying to allocate token stream" << std::endl;
    exit(ANTLR3_ERR_NOMEM);
  }

  parser = ttcn3ParserNew(tokens);
  if(!parser) {
    std::cerr << "Out of memory trying to allocate parser" << std::endl;
    exit(ANTLR3_ERR_NOMEM);
  }
  
  if(argc >= 3 && std::string(argv[2]) == "-t") {
    pANTLR3_UINT8 *tokenNames = parser->pParser->rec->state->tokenNames;
    if(!tokenNames) {
      std::cerr << "Token names not initiated!!!" << std::endl;
      exit(0);
    }

    // estimate the longest token name
    size_t maxTokenLength = 0;
    pANTLR3_VECTOR vector = tokens->getTokens(tokens);
    for(unsigned i=0; i<vector->size(vector); i++) {
      pANTLR3_COMMON_TOKEN token = (pANTLR3_COMMON_TOKEN)vector->get(vector, i);
      maxTokenLength = std::max(maxTokenLength, std::string((char *)tokenNames[token->getType(token)]).size());
    }
    
    for(unsigned i=0; i<vector->size(vector); i++) {
      pANTLR3_COMMON_TOKEN token = (pANTLR3_COMMON_TOKEN)vector->get(vector, i);
      std::cout << "Token[" << std::setw(3) << std::right << token->getType(token) << "] - " <<
        std::setw(maxTokenLength) << std::left << tokenNames[token->getType(token)] << " = " <<
        token->getText(token)->chars << std::endl;
    }
    return 0;
  }
  
  // Override warning messages
  parser->pParser->rec->displayRecognitionError = freettcn::translator::DisplayRecognitionError;
  
  parser->ttcn3Module(parser);
  if(parser->pParser->rec->state->errorCount > 0)
    std::cerr << "The parser returned " << parser->pParser->rec->state->errorCount << " errors, tree walking aborted" << std::endl;
  
  // Must manually clean up
  parser->free(parser);
  tokens->free(tokens);
  lex->free(lex);
  input->close(input);
  
  return 0;
}



// #include <exception>
// #include "file.h"
// #include "module.h"

// using namespace TTCN3;

// static long int errorNum = 0;   /**< The number of errors found during compilation. */

// extern void yyrestart(FILE *input_file); /**< Bison parser state reset. */
// extern int yyparse(TTCN3::CFile &file); /**< Bison parse loop. */

// unsigned long int TTCN3::CNode::refNum = 0;


// /**
//  * @brief Unexpected exception handler.
//  *
//  * Function is called when an unexpected exception is thrown.
//  * Unexpected exception is an exception that is not defined in
//  * function declaration exceptions throw list.
//  */
// static void ExceptionUnexpected()
// {
//   fprintf(stderr, "!Unexpected exception was thrown\n");
//   abort();
// }

// /**
//  * @brief Not caught exception handler. 
//  *
//  * Function is called when an exception is not caught (it is also
//  * true when an exception is thrown inside handling of another
//  * exception).
//  */
// static void ExceptionTerminate()
// {
//   fprintf(stderr, "!An exception was not caught\n");
//   abort();
// }


// /**
//  * @namespace TTCN3
//  * @brief Common namespace for all TTCN-3 compiler tools.
//  */


// /**
//  * @mainpage TTCN-3 Compiler Documentation
//  * @author Mateusz Pusz
//  * @version 0.01
//  *
//  * @section intro_sec Introduction
//  * TTCN-3 (Testing and Test Control Notation version 3) is a flexible and
//  * powerful programming language created for specifying and running test.
//  * It is universal, platform independent and well defined in ETSI
//  * standards. Standard not only describes the interface and behavior of
//  * TTCN-3 language but also defines an interface of compiled TTCN-3 module
//  * for C/C++ and Java environments.
//  *
//  * The problem is that currently there is no free compiler for TTCN-3 language.
//  * The reason of this project is to try provide free compiler for it.
//  * 
//  * @section install_sec Installation
//  * @subsection step1 Step 1: Opening the box
//  * 
//  * etc...
//  */
// int
// main(int argc, char* argv[])
// {
//   extern FILE *yyin; //, *yyout;
// //   extern int yydebug;
// //   yydebug = 1;
  
//   std::set_unexpected(ExceptionUnexpected);
//   std::set_terminate(ExceptionTerminate);

//   // skip over program name
//   ++argv;
//   --argc;
  
//   FILE *fdout = stdout;
// //   if ((fdout = fopen("out.txt", "w")) == 0) {
// //     fprintf(stderr, "Error: File '%s' not found!!!\n", argv[0]);
// //     exit(1);
// //   }
  
//   if (argc) {
//     while (argc) {
//       char const *fileName = argv[0];
      
//       if ((yyin = fopen(fileName, "r")) != 0) {
//         TTCN3::CFile file(fileName);
//         yyrestart(yyin);
//         if (yyparse(file))
//           fprintf(stderr, "Fatal errors occured in '%s'!!!\n", fileName);

//         try {
//           file.Module().DumpTTCN3(fdout);
//         }
//         catch (ENotFound &ex) {
//           fprintf(fdout, "Exception '%s' caught\n", ex.what());
//         }
//         catch (Exception &ex) {
//           fprintf(stderr, "!TTCN3 exception '%s' caught\n", ex.what());
//         }
//         catch (std::exception &ex) {
//           fprintf(stderr, "!C++ standard library exception '%s' caught\n", ex.what());
//         }
//         catch (...) {
//           fprintf(stderr, "!Unknown exception caught\n");
//         }
//       }
//       else {
//         fprintf(stderr, "Error: File '%s' not found!!!\n", fileName);
//         errorNum++;
//       }
      
//       // move to next file
//       ++argv;
//       --argc;
//     }
//   }
//   else {
//     char const * const fileName = "<stdin>";
//     TTCN3::CFile *file = new TTCN3::CFile(fileName);
    
//     yyin = stdin;
//     if (yyparse(*file))
//       fprintf(stderr, "Fatal errors occured in '%s'!!!\n", fileName);
//   }
  
//   if (TTCN3::CNode::refNum)
//     fprintf(stderr, "%lu nodes are still allocated!!!\n", TTCN3::CNode::refNum);
  
//   if (errorNum > 0)
//     return EXIT_FAILURE;
//   else
//     return EXIT_SUCCESS;
// }
