#include <string>
#include <vector>
#include <iostream>
#include <boost/multi_array.hpp>
#include <limits>
#include "hmm.h"
#include "vector2D.h"
using namespace std;

vector2D<double>
HMM::forward(const vector<int>& sentence) const
{
  const uint numWords = sentence.size();
  vector2D<double> trellis(numWords, numTags, 
			   -numeric_limits<double>::infinity());
  
  for (uint tag = 0; tag < numTags; ++tag) {
    trellis(0, tag) = pi[tag] + emission(sentence[0], tag);
  }

  for (uint i = 1; i < numWords; ++i) {
    int word = sentence[i];
    for (uint tag = 0; tag < numTags; ++tag) {
      for (uint prevTag = 0; prevTag < numTags; ++prevTag) {
	trellis(i, tag) = logAdd(trellis(i, tag), trellis(i-1, prevTag) + 
				 transition(prevTag, tag) + 
				 emission(word, tag));
      }
    }
  }
  
  #if DEBUG
  cout << "Forward Table:" << endl;
  printDebug(trellis);
  #endif
  
  return trellis;
}

vector2D<double>
HMM::backward(const vector<int>& sentence) const
{
  const uint numWords = sentence.size();
  vector2D<double> trellis(numWords, numTags, 
			   -numeric_limits<double>::infinity());
  
  for (uint tag = 0; tag < numTags; ++tag) {
    trellis(numWords - 1, tag) = 0.0;
  }
  
  for (int i = numWords - 2; i >= 0; --i) {
    uint nextWord = sentence[i + 1];
    for (uint tag = 0; tag < numTags; ++tag) {
      for (uint nextTag = 0; nextTag < numTags; ++nextTag) {
	trellis(i, tag) = logAdd(trellis(i, tag), trellis(i + 1, nextTag) + 
				 transition(tag, nextTag) + 
				 emission(nextWord, nextTag));
      } 
    }
  }
  
  #if DEBUG
  cout << "Backward Table:" << endl;
  printDebug(trellis);
  #endif

  return trellis;
}

void
HMM::update(void) 
{
  vector2D<double> emisCounts(totalWords, numTags, 
			      -numeric_limits<double>::infinity());
  
  vector2D<double> transCounts(numTags, numTags, 
			       -numeric_limits<double>::infinity());
  
  vector<double> initCounts(numTags, -numeric_limits<double>::infinity());
  vector<double> tagCounts(numTags, -numeric_limits<double>::infinity());
  
  for (auto sentence: sentences) {
    vector2D<double> alpha = forward(sentence);
    vector2D<double> beta = backward(sentence);
    double denom = -numeric_limits<double>::infinity();
    for (uint tag = 0; tag < numTags; ++tag) {
      denom = logAdd(denom, alpha(sentence.size() - 1, tag));
    }
    #if DEBUG
    cout << exp(denom) << endl;
    #endif
    
    for (uint i = 0; i < sentence.size(); ++i) {
      uint word = sentence[i];
      for (uint tag = 0; tag < numTags; ++tag) {
	emisCounts(word, tag) = logAdd(emisCounts(word, tag), 
				       alpha(i, tag) + beta(i, tag) - 
				       denom);
      } 
    }

    for (uint i = 0; i < sentence.size() - 1; ++i) {
      int wordNext = sentence[i + 1];
      for (unsigned int t1 = 0; t1 < numTags; ++t1) {
	for (unsigned int t2 = 0; t2 < numTags; ++t2) {
	  transCounts(t1, t2) = logAdd(transCounts(t1, t2), 
				       alpha(i, t1) + beta(i + 1, t2) + 
				       transition(t1, t2) + 
				       emission(wordNext, t2) - 
				       denom);	  
	}
      }
    }
    
    for (uint tag = 0; tag < numTags; ++tag) {
      int word = sentence[0];
      initCounts[tag] = logAdd(initCounts[tag], pi[tag] + emission(word, tag) + 
				beta(0, tag) - denom);
    }
  }
  
  for (uint word = 0; word < totalWords; ++word) {
    for (uint tag = 0; tag < numTags; ++tag) {
      tagCounts[tag] = logAdd(tagCounts[tag], emisCounts(word, tag));
    }
  }


  // Maximization
  for (uint tag = 0; tag < numTags; ++tag) {
    pi[tag] = initCounts[tag] - log(sentences.size());
  }
  

  for (uint t1 = 0; t1 < numTags; ++t1) {
    for (uint t2 = 0; t2 < numTags; ++t2) {
      transition(t1, t2) = transCounts(t1, t2) - tagCounts[t1];
    }
  }

  for (uint t1 = 0; t1 < numTags; ++t1) {
    double s = -numeric_limits<double>::infinity();
    for (uint t2 = 0; t2 < numTags; ++t2) {
      s = logAdd(s, transition(t1, t2));
    }
    for (uint t2 = 0; t2 < numTags; ++t2) {
      transition(t1, t2) -= s;
    }
  }
  
  for (uint word = 0; word < emission.rows; ++word) {
    for (uint t = 0; t < emission.cols; ++t) {
      emission(word, t) = emisCounts(word, t) - tagCounts[t];
    }
  }

  #if DEBUG
  cout << "New pi: " << endl;
  printDebug(pi);
  cout << "New transition: " << endl;
  printDebug(transition);
  cout << "New emission: " << endl;
  printDebug(emission);
  #endif
}

stack<int> 
HMM::decode(const vector<int>& sentence)
{
  int numWords = sentence.size();
  vector2D<double> trellis(numWords, numTags, 
			   -numeric_limits<double>::infinity());
  vector2D<int> bp(numWords, numTags, -1);
  for (uint t = 0; t < trellis.cols; ++t) {
    trellis(0, t) = pi[t] + emission(sentence[0], t);
  }
  for (uint i = 1; i < trellis.rows; ++i) {
    for (uint t1 = 0; t1 < trellis.cols; ++t1) {
      for (uint t2 = 0; t2 < trellis.cols; ++t2) {
	double tmp = trellis(i-1, t2) + transition(t2, t1);
	if (tmp > trellis(i, t1)) {
	  trellis(i, t1) = tmp;
	  bp(i, t1) = t2; 
	}
      }
      trellis(i, t1) += emission(sentence[i], t1);
    } 
  }

  #if DEBUG
  cout << "Decode Table: " << endl;
  printDebug(trellis);
  #endif

  double vMax = -numeric_limits<double>::infinity();
  int tMax = -1;
  for (uint t = 0; t < numTags; ++t) {
    if (trellis(numWords - 1, t) > vMax) {
      vMax = trellis(numWords - 1, t);
      tMax = t;
    }
  }

  stack<int> sentenceTags;
  for (int i = numWords, t = tMax; i > 0; --i) {
    sentenceTags.push(t);
    t = bp(i-1, t);
  }

  return sentenceTags;
}



