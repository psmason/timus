#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iterator>
#include <string.h>
#include <array>

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

  std::vector<std::string> dictionary(50000, std::string(51,'\0'));
  std::vector<std::string> numberDictionary(50000, std::string(51,'\0'));
}

struct TrieNode
{
  std::array<void*, 10>                                links;     
  int16_t                                              wordIndex;

  TrieNode()
    : wordIndex(-1)
  {
    links.fill(NULL);
  };
};

int getLinkPosition(const char c) {
  return c - '0';
}

struct TrieItr
{
  const TrieNode* node;

  bool has(const char);
  TrieItr next(const char);
};

bool TrieItr::has(const char c)
{
  if ('\0' == c) {
    return node->wordIndex > -1;
  }
  else {
    return NULL != node->links[getLinkPosition(c)];
  }
}

TrieItr TrieItr::next(const char c)
{
  return TrieItr{(TrieNode*) node->links[getLinkPosition(c)]};
}

class Trie
{
public:
  TrieNode d_root;
  std::vector<std::unique_ptr<TrieNode> > d_nodes;

  Trie(const int n);

  void addString(const std::string& s, const int index);
  TrieItr getItr() const;
};

Trie::Trie(const int n)
{}

TrieItr Trie::getItr() const
{
  return TrieItr{&d_root};
}

void Trie::addString(const std::string& s, const int index)
{
  TrieNode* currentNode = &d_root;
  for (const char c : s) {
    auto& links = currentNode->links;
    if (NULL != links[getLinkPosition(c)]) {
      currentNode = (TrieNode*) currentNode->links[getLinkPosition(c)];
    }
    else {
      d_nodes.push_back(std::unique_ptr<TrieNode>(new TrieNode));
      currentNode->links[getLinkPosition(c)] = (void*) d_nodes.back().get();
      currentNode = d_nodes.back().get();
    }
  }
  // marking the end of the trie string
  currentNode->wordIndex = index;
}

int getOffset(const Trie& trie
              , const WordSequence& path)
{
  int offset = 0;
  for (const auto& index : path) {
    offset += numberDictionary[index].length();
  }
  return offset;
}

using TraversalTable = std::unordered_map<int16_t, WordSequence >;
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

void printSolution(const Trie& trie
                   , const WordSequence& solution)
{
  if (solution.empty()) {
    std::cout << "No solution." << std::endl;
    return;
  }
  
  for (auto itr = solution.begin(); itr != solution.end(); ++itr) {
    if (itr != solution.begin()) {
      std::cout << " ";
    }
    std::cout << dictionary[*itr];
  }
  std::cout << std::endl;
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
    const auto& lhs = numberDictionary[stack.back()];
    const auto& rhs = numberDictionary[path.back()];
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

void copyToDictionary(const std::string& s
                     , char entry[50])
{
  s.copy(entry, s.length());
  entry[s.length()] = '\0';
}

void runNumber(const std::string& number)
{
  clearTraversalTable();
  int wordCount;
  std::cin >> wordCount;

  Trie trie(wordCount);
  for (;wordCount>0;--wordCount) {
    std::string word;
    std::cin >> word;
    dictionary[wordCount-1] = word;

    const auto numberString = getNumberString(word);
    numberDictionary[wordCount-1] = numberString;

    trie.addString(numberString, wordCount-1);
  }

  WordSequence solution;
  findSequence(number, trie, solution);  
  printSolution(trie, solution);
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
