#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "FileManager.h"
#include "Lexer.h"
#include <stack>
#include <string>
#include <unordered_map>

class Define {};

class PreProcessor {
  public:
    PreProcessor(FileManager &fm);

    void getNextToken(void);

  private:
    FileManager &_fm;

    // to keep track of file that have ben included
    // and define that have ben expend
    std::stack<Lexer> _tokenStack;

    // keep track of all the define
    // _defineList["foo"].expend will expend the token
    // and put the token onto the tokenStack
    std::unordered_map<std::string, Define> _defineList;

    // use to handle #if #ifndef #ifelse #else
    enum Conditional { branchTaken, branchNotTaken };
    std::stack<Conditional> _conditional;

    // #if defined(FOO) || BAR
    // the func take "defined(FOO) || BAR"
    bool _evaluateConditional(std::string conditional);

    // #	define FOO(x) bar(x);    \n
    // this func take "FOO(x) bar(x);    "
    Define _parseDefine(std::string define);

    // # include <foo.h> tewrwrw
    // this func take "<foo.h> tewrwrw"
    FileManager::File _parseInclude(std::string include);
};

#endif /* PREPROCESSOR_H */
