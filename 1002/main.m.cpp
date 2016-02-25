#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

namespace {
  std::vector<std::vector<char> > digitmap;
}

void initDigitMap()
{
  using Chars = std::vector<char>;
  digitmap.push_back(Chars{'o','q','z'});
  digitmap.push_back(Chars{'i','j'});
  digitmap.push_back(Chars{'a','b','c'});
  digitmap.push_back(Chars{'d','e','f'});
  digitmap.push_back(Chars{'g','h'});
  digitmap.push_back(Chars{'k','l'});
  digitmap.push_back(Chars{'m','n'});
  digitmap.push_back(Chars{'p','r','s'});
  digitmap.push_back(Chars{'t','u','v'});
  digitmap.push_back(Chars{'w','x','y'});
}

struct TrieNode
{
  std::unordered_map<char, std::unique_ptr<TrieNode> > links;
};

struct TrieItr
{
  TrieNode* node;
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
  // data
  TrieNode d_root;

  void addString(const std::string& s);
  bool hasString(const std::string& s);
};

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

int main()
{
  initDigitMap();
  Trie trie;
  trie.addString("it");  
  trie.addString("your");
  trie.addString("pizza");

  testString(trie, "it", true);
  testString(trie, "your", true);
  testString(trie, "pizza", true);
  testString(trie, "yogurt", false);
}
