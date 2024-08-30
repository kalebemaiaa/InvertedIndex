#include "./modules/Interface.hpp"

int main()
{
  Trie *myTrie = new Trie();
  std::string fn = "C:\\Users\\b47133\\Documents\\InvertedIndex\\tests\\test1.txt";
  std::string p = "C:\\Users\\b47133\\Downloads\\dhbb-nlp-master\\dhbb-nlp-master\\raw";

  myTrie->indexByFile(fn, p);

  Interface myInterface(myTrie);

  myInterface.listQueries("./results/resultsCPP.txt", {Query::wQuery("estudou"),
                                                       Query::wQuery("candidato"),
                                                       Query::andQuery({Query::wQuery("estudou"),
                                                                        Query::wQuery("candidato")}),
                                                       Query::orQuery({Query::wQuery("estudou"),
                                                                       Query::wQuery("candidato")}),
                                                       Query::nearQuery("exerceu", "cargo", 3)});
                              
  myInterface.writeTest("./tests/test1.out", Query::orQuery({Query::wQuery("presidente"), Query::wQuery("ministro")}), "./tests/test1.txt", p);
  return 0;
}
