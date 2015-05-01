#ifndef _HMM_H
#define _HMM_H
#include <vector>
#include <stack>
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>
#include "vector2D.h"
using namespace std;
class HMM {
 private:
  vector<double> pi;
  vector2D<double> transition;
  vector2D<double> emission;
  vector<vector<int>> sentences;
  const uint numTags;
  const uint totalWords;
  uint maxIter = 50; 
  static inline double logAdd(double logX, double logY) {
    if (logY > logX) {
      double temp = logX;
      logX = logY;
      logY = temp;
    }

    if (logX == -numeric_limits<double>::infinity()) {
      return logX;
    }

    double negDiff = logY - logX;
    if (negDiff < -20) {
      return logX;
    }
  
    return logX + log(1.0 + exp(negDiff));
  }

  static inline void toLog(vector2D<double>& vec2d) {
    for (uint i = 0; i < vec2d.rows; ++i) {
      for (uint j = 0; j < vec2d.cols; ++j) {
	vec2d(i, j) = log(vec2d(i, j));
      }
    }
  }

  static inline void toLog(vector<double>& vec) {
    for (uint i = 0; i < vec.size(); ++i) {
      vec[i] = log(vec[i]);
    }
  }

  #ifdef DEBUG
  template<typename T>
  static void printDebug(const vector2D<T>& vec2d) {
    for (uint i = 0; i < vec2d.rows; ++i) {
      for (uint j = 0; j < vec2d.cols; ++j) {
	cout << exp(vec2d(i, j)) << "\t";
      }
      cout << std::endl;
    }
    cout << endl;
  }

  template<typename T>
  static void printDebug(const vector<T>& vec) {
    for (uint i = 0; i < vec.size(); ++i) {
      cout << exp(vec[i]) << "\t";
    }
    cout << endl;
    cout << endl;
  }
  #endif

 public:
  void setMaxIter(uint _maxIter) {
    maxIter = _maxIter;
  }
  HMM(const vector<double>& _pi, 
      const vector2D<double>& _transition,
      const vector2D<double>& _emission, 
      const vector<vector<int>>& _sentences) : pi(_pi), 
    transition(_transition), emission(_emission), sentences(_sentences), 
    numTags(pi.size()), totalWords(emission.rows) {
    
    // Convert to log space
    toLog(pi);
    toLog(emission);
    toLog(transition);

    #ifdef DEBUG 
      cout << "Pi: " << endl;
      printDebug(pi);
      cout << "Emission: " << endl;
      printDebug(emission);
      cout << "Transition: " << endl;
      printDebug(transition);
    #endif
  }
  
  vector2D<double> forward(const vector<int>& sentence) const;
  vector2D<double> backward(const vector<int>& sentence) const; 
  void update();
  stack<int> decode(const vector<int>& sentence);

  void train() {
    for (int i = 0; i < maxIter; ++i) {
      cout << i + 1 << " iteration passed..." << endl;
      update();
    }
  }


  void test(const vector<vector<int>>& goldTags, 
	    const vector<string> words, const vector<string>& tags) {
    vector2D<int> confMatrix(numTags, numTags, 0);
    ofstream ofs("result.txt", std::ofstream::out);
    uint total = 0;
    for (uint i = 0; i < sentences.size(); ++i) {
      auto sentenceTags = decode(sentences[i]);
      assert(sentenceTags.size() == goldTags[i].size());
      total += sentenceTags.size();
      for (uint j = 0; j < goldTags[i].size(); ++j) {
	confMatrix(goldTags[i][j], sentenceTags.top()) += 1;
	ofs << words[sentences[i][j]] + "_" + tags[sentenceTags.top()] << " ";
	sentenceTags.pop();
      }
      ofs << "\n";
    }
    uint acc = 0;
    for (uint tag = 0; tag < numTags; ++tag) {
      acc += confMatrix(tag, tag);
    }
    
    cout << "accuracy: " << acc * 100.0 / total << "%" << endl;
    cout << "Cofusion Matrix: " << endl;
    cout << "\t";
    for (uint i = 0; i < tags.size(); ++i) {
      cout << tags[i] << "\t";
    }
    cout << endl;
    for (uint i = 0; i < confMatrix.rows; ++i) {
      cout << tags[i] << "\t";
      for (uint j = 0; j < confMatrix.cols; ++j) {
	cout << confMatrix(i, j) << "\t";
      }
      cout << endl;
    }
    ofs.close();
  }
};
#endif
