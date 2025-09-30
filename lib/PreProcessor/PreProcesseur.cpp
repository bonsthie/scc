#include "scc/PreProcessor/PreProcessor.h"

using namespace scc;

PreProcessor::PreProcessor(File &F) {
    FileScope.push_back(F.getFileID());
    addNewTokenStream(F);
}

bool PreProcessor::next(Token &Tok) {

}
