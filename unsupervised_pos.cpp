#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "hmm.h"
#include "vector2D.h"
using namespace std;

void 
readLexicon(const char* fileName, vector<string>& words, 
	    vector<string>& tags, unordered_map<string, int>& tagCounts, 
	    unordered_map<string, vector<string>>& tokens)
{
  ifstream lexicon(fileName);
  string line;
  while (getline(lexicon, line)) {
    stringstream ss(line);
    string word;
    ss >> word;
    words.push_back(word);
    string tag;
    while (ss >> tag) {
      tagCounts[tag] += 1;
      tokens[word].push_back(tag);
    }
  }
  for (auto it = tagCounts.begin(); it != tagCounts.end(); ++it ) {
    tags.push_back(it->first);
  }
  lexicon.close();
}

vector<double>
initPi(const vector<string>& tags) 
{
  return vector<double>(tags.size(), 1.0/tags.size());
}

vector2D<double>
initTransition(const vector<string>& tags)
{
  return vector2D<double>(tags.size(), tags.size(), 1.0 / tags.size());
}

 
vector2D<double>
initEmission(unordered_map<string, vector<string>>& tokens, 
	     unsigned int tagSize, unsigned int wordSize, 
	     unordered_map<string, int>& mapTags, 
	     unordered_map<string, int>& mapWords) 
{
  vector2D<double> emission(wordSize, tagSize, 0);
  for (auto token : tokens) {
    for (auto tag : token.second) {
      emission(mapWords[token.first], mapTags[tag]) = 1.0 / token.second.size();
    }
  } 
  return emission;
}

unordered_map<string, int> 
toMap(const vector<string>& vs)
{
  unordered_map<string, int> myMap;
  for (unsigned int i = 0; i < vs.size(); ++i) {
    myMap[vs[i]] = i;
  }
  return myMap;
}

vector<vector<int>>
readTrainFile(const char* fileName, unordered_map<string, int>& mapWords)
{
  vector<vector<int>> sentences; 
  ifstream train(fileName);
  string line;
  while (getline(train, line)) {
    vector<int> sentence;
    stringstream ss(line);
    string word;
    while (ss >> word) {
      sentence.push_back(mapWords[word]);
    }
    sentences.push_back(sentence);
  }
  train.close();
  return sentences;
}

vector<vector<int>>
readGoldFile(const char* fileName, unordered_map<string, int>& mapTags)
{
  vector<vector<int>> sentences; 
  ifstream gold(fileName);
  string line;
  while (getline(gold, line)) {
    vector<int> sentence;
    stringstream ss(line);
    string word;
    while (ss >> word) {
      string tag = word.substr(word.find("_") + 1, word.length());
      sentence.push_back(mapTags[tag]);
    }
    sentences.push_back(sentence);
  }
  gold.close();
  return sentences;
}


int 
main(int agrc, char* argv[]) 
{
  vector<string> words;
  vector<string> tags;
  unordered_map<string, int> tagCounts;
  unordered_map<string, vector<string>> tokens;
  readLexicon("HW6.lexicon.txt", words, tags, tagCounts, tokens);
  vector<double> pi = initPi(tags);

  vector2D<double> transition = initTransition(tags);

  unordered_map<string, int> mapWords = toMap(words);
  unordered_map<string, int> mapTags = toMap(tags);
  
  vector2D<double> emission = initEmission(tokens, tags.size(), words.size(), 
					   mapTags, mapWords);
  
  vector<vector<int>> sentences = readTrainFile("HW6.unlabeled.txt", mapWords);
  vector<vector<int>> goldTags = readGoldFile("HW6.gold.txt", mapTags);

  HMM hmm(pi, transition, emission, sentences);
  hmm.setMaxIter(52);
  hmm.train();
  hmm.test(goldTags, words, tags);
  return 0;
} 

