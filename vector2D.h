#ifndef _VECTOR2D_H
#define _VECTOR2D_H
#include <vector>
#include <iostream>
#include <cmath>
template<typename T>
class vector2D {
 public:
  const uint rows;
  const uint cols;
  vector2D(uint _rows, uint _cols, T _defaultVal): rows(_rows), 
    cols (_cols), elems(rows * cols, _defaultVal) { }
  
  vector2D(vector2D&&) = default; // Move
  vector2D& operator=(vector2D&&) = default;
  
  vector2D(vector2D const&) = default; // Copy
  vector2D& operator=(vector2D const&) = default;
  ~vector2D() = default;
  
  T& operator()(uint i, uint j) { 
    return elems[i*cols + j]; 
  }
  T operator()(uint i, uint j) const { 
    return elems[i*cols + j]; 
  }

  friend std::ostream& operator<<(std::ostream& os, const vector2D<T>& vec2d) {
    os << "Size: " << vec2d.elems.size() << std::endl;
    for (uint i = 0; i < vec2d.rows; ++i) {
      for (uint j = 0; j < vec2d.cols; ++j) {
	os << vec2d(i, j) << "\t";
      }
      os << std::endl;
    }
    return os;
  }
 private:
  std::vector<T> elems; // the elements
};
#endif
