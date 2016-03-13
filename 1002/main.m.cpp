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
}

struct TrieNode
{
  std::unordered_map<char, std::unique_ptr<TrieNode> > links;
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

  void addString(const std::string& s);
  bool hasString(const std::string& s);
  TrieItr getItr() const;
};

TrieItr Trie::getItr() const
{
  return TrieItr{&d_root};
}

void Trie::addString(const std::string& s)
{
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
}

bool Trie::hasString(const std::string& s)
{
  TrieItr itr{&d_root};
  for (const char c : s) {    
    if (!itr.has(c)) {
      return false;
    }
    itr = itr.next(c);    
  }
  return itr.has('\0');
}

void testString(Trie& trie, const std::string& s, bool expected)
{
  if (trie.hasString(s) != expected) {
    std::cout << "Unexpected result for string " << s << std::endl;
  }
}

int getOffset(const std::vector<std::string>& path)
{
  int offset = 0;
  for (const auto& s : path) {
    offset += s.length();
  }
  return offset;
}

void printSequence(const std::vector<std::string>& sequence
                   , const std::string& category)
{
  std::cout << category << ":: ";
  std::ostream_iterator<std::string> itr(std::cout, " ");
  std::copy(sequence.begin(), sequence.end(), itr);
  std::cout << std::endl;
}

void traverseNumber(const std::string& number
                    , const Trie& trie
                    , std::vector<std::string>& stack
                    , std::vector<std::string>& path
                    , std::vector<std::string>& solution)
{
  const auto offset = getOffset(path);
  if (number.size() == offset) {
    if (solution.empty()) {
      solution = path;
      return;
    }
    else if (path.size() < solution.size()) {
      solution = path;
    }
    return;
  }

  std::ostringstream oss;
  auto trieItr = trie.getItr();
  for (auto itr = number.begin() + getOffset(path)
         ; itr != number.end()
         ; ++itr) {

    if (trieItr.has('\0')) {
      stack.push_back(oss.str());
    }

    if (trieItr.has(*itr)) {
      oss << *itr;
      trieItr = trieItr.next(*itr);
    }
    else {
      return;
    }
  }

   if (trieItr.has('\0')) {
     stack.push_back(oss.str());
   }
}

void maintainTraversal(std::vector<std::string>& stack
                       , std::vector<std::string>& path)
{
  while (true) {
    if (stack.empty() || path.empty()) {
      return;
    }
    if (stack.back() == path.back()) {
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
                  , std::vector<std::string>& solution)
{
  std::vector<std::string> stack;
  std::vector<std::string> path;

  traverseNumber(number, trie, stack, path, solution);
  //printSequence(stack, "stack");

  while (!stack.empty()) {
    path.push_back(stack.back());
    //printSequence(path, "path");
    //printSequence(stack, "stack");
    traverseNumber(number, trie, stack, path, solution);
    maintainTraversal(stack, path);
  }
}

using WordMap = std::unordered_map<std::string, std::string>;

void printSolution(const std::vector<std::string>& solution
                   , const WordMap& wordMap)
{
  if (solution.empty()) {
    std::cout << "No solution." << std::endl;
    return;
  }
  
  for (const auto& s : solution) {
    std::cout << wordMap.find(s)->second << " ";
  }
  std::cout << std::endl;
}

void runNumber(const std::string& number)
{
  std::cout << "running for number " << number << std::endl;
  int wordCount;
  std::cin >> wordCount;

  WordMap wordMap;

  Trie trie;
  for (;wordCount>0;--wordCount) {
    std::string word;
    std::cin >> word;
    const auto numberString = getNumberString(word);
    wordMap.emplace(numberString, word);
    trie.addString(getNumberString(word));
  }

  std::vector<std::string> solution;
  findSequence(number, trie, solution);  
  printSolution(solution, wordMap);
}

int main()
{
  std::string number;
  std::cin >> number;
  runNumber(number);
  /*
  Trie trie;
  trie.addString("it");  
  trie.addString("your");
  trie.addString("pizza");

  testString(trie, "it", true);
  testString(trie, "your", true);
  testString(trie, "pizza", true);
  testString(trie, "yogurt", false);
  */

  
}
