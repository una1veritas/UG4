#ifndef LAYOUT_HPP
#define LAYOUT_HPP

namespace GridLayout {

  //
  // [Tamplate]
  // TYPE : type of a point-element.
  // 
  // [Class] Point Set
  //
  // <Constructor>
  // std::istream& : point-set-text-stream
  // const TYPE*, const size_t : pointset-array and length-of-array
  //
  template<typename TYPE>
  class PointSet {
  public:
    PointSet();
    PointSet(std::istream &sIn);
    PointSet(const TYPE *pset, const size_t length);
  };

  //
  // Grid Layout Function
  // IN - pointset : input-pointset.
  // IN - gridsize : gridsize.
  //
  // OUT - pointset : output-pointset.
  // OUT - error-value : 0 on success, -1 on failed.
  //
  template<typename TYPE>
  int GridLayout(PointSet<TYPE> *pointset, const TYPE gridsize);
}

#endif
