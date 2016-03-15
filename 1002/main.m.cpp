#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iterator>

namespace {
  char getNumber(const char c) 
  {
    switch(c) {
    case 'i':
    case 'j':
      return '1';
    case 'a':
    case 'b':
    case 'c':
      return '2';
    case 'd':
    case 'e':
    case 'f':
      return '3';
    case 'g':
    case 'h':
      return '4';
    case 'k':
    case 'l':
      return '5';
    case 'm':
    case 'n':
      return '6';
    case 'p':
    case 'r':
    case 's':
      return '7';
    case 't':
    case 'u':
    case 'v':
      return '8';
    case 'w':
    case 'x':
    case 'y':
      return '9';
    case 'o':
    case 'q':
    case 'z':
      return '0';    
    }
  }

  const std::string getNumberString(const std::string& s)
  {
    std::ostringstream oss;
    for (const auto c: s) {
      oss << getNumber(c);
    }
    return oss.str();
  }

  using WordSequence = std::vector<int16_t>;
}

struct TrieNode
{
  std::unordered_map<char, std::unique_ptr<TrieNode> > links;
  int                                                  wordIndex;
};

struct TrieItr
{
  const TrieNode* node;

  bool has(const char);
  TrieItr next(const char);
};

bool TrieItr::has(const char c)
{
  return node->links.count(c) > 0;
}

TrieItr TrieItr::next(const char c)
{
  auto itr = node->links.find(c);
  return TrieItr{itr->second.get()};
}

class Trie
{
public:
  TrieNode d_root;
  std::vector<std::string> d_words;
  Trie(const int n);

  void addString(const std::string& s);
  TrieItr getItr() const;
};

Trie::Trie(const int n)
{
  d_words.reserve(n);
}

TrieItr Trie::getItr() const
{
  return TrieItr{&d_root};
}

void Trie::addString(const std::string& s)
{
  d_words.push_back(s);
  TrieNode* currentNode = &d_root;
  for (const char c : s) {
    auto& links = currentNode->links;
    auto itr = links.find(c);
    if (links.end() == itr) {
      auto p = links.emplace(c, std::unique_ptr<TrieNode>(new TrieNode));
      currentNode = p.first->second.get();      
    }
    else {
      currentNode = itr->second.get();
    }
  }
  // marking the end of the trie string
  currentNode->links.emplace('\0', std::unique_ptr<TrieNode>(nullptr));
  currentNode->wordIndex = d_words.size()-1;
}

int getOffset(const Trie& trie
              , const WordSequence& path)
{
  int offset = 0;
  for (const auto& index : path) {
    offset += trie.d_words[index].length();
  }
  return offset;
}

using TraversalTable = std::unordered_map<int, WordSequence >;
TraversalTable traversalTable;

bool checkTraversalTable(const int offset
                         , WordSequence& stack)
{
  if (traversalTable.find(offset) == traversalTable.end()) {
    return false;
  }
  
  const auto& words = traversalTable[offset];
  for (const auto& word : words) {
    stack.push_back(word);
  }

  return true;
}

void updateTraversalTable(const int offset
                          , const WordSequence& words)
{
  traversalTable[offset] = words;
}

void clearTraversalTable()
{
  traversalTable.clear();
}

void traverseNumber(const std::string& number
                    , const Trie& trie
                    , WordSequence& stack
                    , WordSequence& path
                    , WordSequence& solution)
{
  const auto offset = getOffset(trie, path);
  if (number.size() == offset) {
    if (solution.empty()) {
      solution = path;
    }
    else if (path.size() < solution.size()) {
      solution = path;
    }
    return;
  }
  
  if (checkTraversalTable(offset, stack)) {
    // used an already computed search.
    return;
  }

  // running a new search to fill traversal table
  WordSequence words;

  auto trieItr = trie.getItr();
  for (auto itr = number.begin() + offset
         ; itr != number.end()
         ; ++itr) {

    if (trieItr.has('\0')) {
      // a possible solution if path + 1 <= solution

      if (!solution.empty() && path.size() + 1 >= solution.size()) {
        // no need to traverse since an potential paths would be bigger than the solution.
      }
      else {
        words.push_back(trieItr.node->wordIndex);
        //stack.push_back(trieItr.node->wordIndex);
      }
    }

    if (trieItr.has(*itr)) {
      trieItr = trieItr.next(*itr);
    }
    else {
      for (const auto& word : words) {
        stack.push_back(word);
      }
      updateTraversalTable(offset, words);
      return;
    }
  }

   if (trieItr.has('\0')) {
     words.push_back(trieItr.node->wordIndex);
     //stack.push_back(trieItr.node->wordIndex);
   }

   updateTraversalTable(offset, words);

   // stack update
   for (const auto& word : words) {
     stack.push_back(word);
   }
}

void maintainTraversal(const Trie& trie
                       , WordSequence& stack
                       , WordSequence& path)
{
  while (true) {
    if (stack.empty() || path.empty()) {
      return;
    }
    const auto& lhs = trie.d_words[stack.back()];
    const auto& rhs = trie.d_words[path.back()];
    if (lhs == rhs) {
      stack.pop_back();
      path.pop_back();
    }
    else {
      return;
    }
  }
}

void findSequence(const std::string& number
                  , const Trie& trie
                  , WordSequence& solution)
{
  WordSequence stack;
  WordSequence path;

  traverseNumber(number, trie, stack, path, solution);

  while (!stack.empty()) {
    path.push_back(stack.back());

    if (!solution.empty() && path.size() >= solution.size()) {
      // no need to traverse since an potential paths would be bigger than the solution.
    }
    else {
      traverseNumber(number, trie, stack, path, solution);
    }

    maintainTraversal(trie, stack, path);
  }
}

using WordMap = std::unordered_map<std::string, std::string>;

void printSolution(const Trie& trie
                   , const WordSequence& solution
                   , const WordMap& wordMap)
{
  if (solution.empty()) {
    std::cout << "No solution." << std::endl;
    return;
  }
  
  for (auto itr = solution.begin(); itr != solution.end(); ++itr) {
    if (itr != solution.begin()) {
      std::cout << " ";
    }
    const auto& numberWord = trie.d_words[*itr];
    std::cout << wordMap.find(numberWord)->second;
  }
  std::cout << std::endl;
}

void runNumber(const std::string& number)
{
  clearTraversalTable();
  int wordCount;
  std::cin >> wordCount;

  WordMap wordMap;
  wordMap.reserve(wordCount);

  Trie trie(wordCount);
  for (;wordCount>0;--wordCount) {
    std::string word;
    std::cin >> word;
    const auto numberString = getNumberString(word);
    wordMap.emplace(numberString, word);
    trie.addString(getNumberString(word));
  }

  WordSequence solution;
  findSequence(number, trie, solution);  
  printSolution(trie, solution, wordMap);
}

int main()
{
  while (true) {
    std::string number;
    std::cin >> number;
    if ("-1" == number) {
      break;
    }
    runNumber(number);
  }
}
