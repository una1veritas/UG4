#include "layout.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <cmath>
#include <list>

namespace {
  template <typename TYPE>
  TYPE modulo(TYPE a, TYPE b) {
    return a % b;
  }
  template <>
  float modulo(float a, float b) {
    return std::fmod(a,b);
  }
  template <>
  double modulo(double a, double b) {
    return std::fmod(a,b);
  }
  template <>
  int modulo(int a, int b) {
    return a % b;
  }
  
  template<typename TYPE>
  class Point {
    TYPE x, y;

    inline TYPE calc_u(TYPE a, int grid) {
      return modulo<TYPE>(std::abs(a), (TYPE)grid) * (a > 0 ? -1 : 1);
    }
    
    inline TYPE calc_v(TYPE a, TYPE u, int grid) {
      return (grid - std::abs(u)) * (a > 0 ? 1 : -1); 
    }

    inline TYPE ifswap(TYPE v, TYPE a, TYPE b) {
      return (v == a) ? b : a;
    }

  public:
    inline static Point right_top(Point a, Point b) {
      return Point(std::max(a.x, b.x), std::max(a.y, b.y));
    }

    Point() : x(0), y(0) {}
    Point(TYPE x, TYPE y) : x(x), y(y) {}
    Point(const Point &p) : x(p.x), y(p.y) {}
    ~Point() {}
    TYPE getX () const { return x ; }
    TYPE getY () const { return y ; }
    void setX (TYPE x) { this-> x = x; }
    void setY (TYPE y) { this-> y = y; }
    Point trans(Point px, Point py, int grid) {
      Point rt = right_top(px, py);
      Point t(*this - rt);

      TYPE ux = calc_u(t.x, grid), vx = calc_v(t.x, ux, grid);
      TYPE uy = calc_u(t.y, grid), vy = calc_v(t.y, uy, grid);
      
      px = px - rt;
      py = py - rt;
      
      Point r = Point(ux,uy);
      r.y = std::abs(uy) < std::abs(vy) ? uy : vy;
      r.x = std::abs(ux) < std::abs(vx) ? ux : vx;
      
      Point tmp = t + r;
      if(tmp.is_zero() && px.is_zero() && py.is_zero()) {
        if(t.x < 0) {
          r.y = ifswap(r.y, uy, vy);
        } else if(t.y < 0) {
          r.x = ifswap(r.x, ux, vx);
        } else if( std::abs(ifswap(r.x, ux, vx)) < std::abs(ifswap(r.y, uy, vy)) ) {
          r.x = ifswap(r.x, ux, vx);
        } else {
          r.y = ifswap(r.y, uy, vy);
        }
      } else if(tmp.x <= 0 && tmp.y <= 0) {
        if(t.x >= 0) {
          r.x = ifswap(r.x, ux, vx);
        }
        if(t.y >= 0) {
          r.y = ifswap(r.y, uy, vy);
        }
      }
      
      return r;
    }
    TYPE sum() const { return x + y; }
    TYPE length() const { return abs(x) + abs(y); }
    bool is_zero() const { return x == 0 && y == 0; }
    bool operator ==(Point &p) { return (x == p.x) && (y == p.y); }
    Point &operator =(Point p) { x=p.x; y=p.y; return *this; }
    Point operator +(Point p) { Point q(x + p.x, y + p.y); return q; }
    Point &operator +=(Point p) { x += p.x; y += p.y; return *this; }
    Point &operator -=(Point p) { x -= p.x; y -= p.y; return *this; }
    Point operator -(Point &p) { Point q(x - p.x, y - p.y); return q; }
    void print(std::ostream &out) { out << x << " " << y; }
  };
  
  template<typename TYPE> std::ostream &operator <<(std::ostream &out, Point<TYPE> &p) {
    p.print(out);
    return out;
  }

  template<typename TYPE>
  Point<TYPE> rectMultiPoints(int n, TYPE grid_size) {
    int ix = 0, iy = 0;
    for(int j=1,k=1; j<n; ++ix) {
      k += 2;
      j += k;
    }
    for(int j=2,k=2; j<n; ++iy) {
      k += 2;
      j += k;
    }
    return Point<TYPE>(ix * grid_size, iy * grid_size);
  }

  template<typename TYPE>
  TYPE sumMultiPointsDistance(int n, TYPE grid_size) {
    return rectMultiPoints(n, grid_size).length();
  }
}

namespace gridlayout {

  template<typename TYPE>
  class PointSet : public std::vector<Point<TYPE> > {
    typedef Point<TYPE> POINT_T;
    typedef typename std::vector<POINT_T>::iterator iterator;
  public:
    PointSet() : std::vector<POINT_T>() {}
    ~PointSet() {}
    
    PointSet(std::istream &in) : std::vector<POINT_T>() {
      TYPE x, y;
      in >> x;
      in >> y;
      while(!in.eof()) {
        std::vector<POINT_T>::push_back(POINT_T(x,y));
        in >> x;
        in >> y;
      }
    }

    PointSet(PointSet &set) : std::vector<POINT_T>(set.size()) {
      for(int i=0; i<set.size(); ++i) {
        std::vector<POINT_T>::at(i) = set[i];
      }
    }
    
    PointSet(int size) : std::vector<POINT_T>(size) {}
    void print(std::ostream &out) {
      for(iterator i = std::vector<POINT_T>::begin();
          i != std::vector<POINT_T>::end();
          i++)
      {
        i->print(out);
        out << std::endl;
      }
    }
    
    PointSet(const TYPE *array, const size_t maxlen) : std::vector<POINT_T>(maxlen) {
      for(size_t i=0; i<maxlen; i+=2)
      {
        std::vector<POINT_T>::at(i).setX(array[i]);
        std::vector<POINT_T>::at(i+1).setY(array[i+1]);
      }
    }
    
    bool checkIndependent() {
      bool check = true;
      for(int i = 0; i < std::vector<POINT_T>::size(); i++) {
        for(int j = i; j < std::vector<POINT_T>::size(); j++) {
          if(!(i==j) && (std::vector<POINT_T>::at(i) == std::vector<POINT_T>::at(j))) {
            std::cerr << "ERROR : GRID-LAYOUT FAILED." << std::endl;
            std::cerr << i << " : " << std::vector<POINT_T>::at(i) << " | "
                      << j << " : " << std::vector<POINT_T>::at(j) << std::endl;
            check = false;
          }
        }
      }
      return check;
    }
    
    PointSet &operator=(PointSet &set) {
      for(int i=0; i<std::vector<POINT_T>::size(); ++i) {
        std::vector<POINT_T>::at(i) = set[i];
      }
      return *this;
    }
  };

  template<typename TYPE>
  std::ostream &operator <<(std::ostream &out, PointSet<TYPE> &p) {
    p.print(out);
    return out;
  }
}

namespace {
  using namespace gridlayout;
 
  template<typename TYPE>
  class compare_XY {
    PointSet<TYPE> &set;
  public:
    compare_XY(PointSet<TYPE> &set) : set(set) {}
      
    bool operator ()(const TYPE &a, const TYPE &b) const {
      if(set[a].getX() == set[b].getX()) {
        return set[a].getY() < set[b].getY();
      }
      return set[a].getX() < set[b].getX();
    }
  };
  
  template<typename TYPE>
  class compare_YX {
    PointSet<TYPE> &set;
  public:
    compare_YX(PointSet<TYPE> &set) : set(set) {}
      
    bool operator ()(const TYPE &a, const TYPE &b) const {
      if(set[a].getY() == set[b].getY()) {
        return set[a].getX() < set[b].getX();
      }
      return set[a].getY() < set[b].getY();
    }
  };
  
  template<typename TYPE>
  class PointSequence : public std::vector<int> {
    typedef typename std::vector<int>::iterator iterator;
    PointSet<TYPE> &set;
  public:
    ~PointSequence() {}
    
    PointSequence(PointSet<TYPE> &set, bool sortx)
    : std::vector<int>(set.size()), set(set) {
      int j=0;
      for(iterator i=std::vector<int>::begin(); i!=std::vector<int>::end(); ++i) {
        *i = j++;
      }
      
      if(sortx) {
        std::sort(std::vector<int>::begin(),
                  std::vector<int>::end(),
                  compare_XY<TYPE>(set));
      } else {
        std::sort(std::vector<int>::begin(),
                  std::vector<int>::end(),
                  compare_YX<TYPE>(set));
      }
    }
    
    Point<TYPE> &ref(int i) {
      return set.at(at(i));
    }
    
    void print(std::ostream &out) {
      for(iterator i=std::vector<int>::begin(); i!=std::vector<int>::end(); ++i) {
        out << *i << " : " << set[*i] << std::endl;
      }
    }
  };
  
  template<typename TYPE>
  std::ostream &operator <<(std::ostream &out, PointSequence<TYPE> &p) {
    p.print(out);
    return out;
  }

  template <typename TYPE>
  class grid : public std::vector<TYPE> {
    typedef typename std::vector<TYPE>::iterator iterator;
    
    int grid_size;
  public:
    grid() : std::vector<TYPE>(), grid_size(0) {}
    
    grid(int size) : std::vector<TYPE>(size*size), grid_size(size) {}
    
    TYPE &ref(int x, int y) {
      return std::vector<TYPE>::at(x + y * grid_size);
    }
    
    void print(std::ostream &out) {
      int j=0;
      for(iterator i=std::vector<TYPE>::begin(); i!=std::vector<TYPE>::end(); i++) {
        out << "  <" << *i << ">";
        if(++j % grid_size == 0) {
          out << std::endl;
        }
      }
    }
  };
  
  template<typename TYPE>
  std::ostream &operator <<(std::ostream &out, grid<TYPE> &p) {
    p.print(out);
    return out;
  }

  template <typename TYPE>
  class GridLayoutMan {
    typedef Point<TYPE> POINT_T;
    typedef PointSet<TYPE> PSET_T;
    typedef PointSequence<TYPE> PSEQ_T;
    typedef std::vector< std::list< Point<int> > > RDIC_T;
    enum DIRECTION_T {
      RIGHT = 1,
      TOP = 2,
      RIGHT_TOP = 3
    };
    enum SQUARE_T {
      NONE = 0,
      SINGLE = 1,
      DOUBLE = 2
    };

    TYPE grid_size; 

    int checkPointPair(int i, int j, PSEQ_T &x, PSEQ_T &y) {
      POINT_T &p = x.ref(i), &q = y.ref(j);
      if(p == q) {
        return SINGLE;
      }
      if(p.getX() >= q.getX() && p.getY() <= q.getY()) {
        return DOUBLE;
      }
      return NONE;
    }
    
    bool checkMultiPoints(int i, PSEQ_T &s) {
      if(i+1 < s.size() && s.ref(i) == s.ref(i+1)) {
        return true;
      }
      if(0 <= i-1 && s.ref(i) == s.ref(i-1)) {
        return true;
      }
      return false;
    }
    
    int rewindMultiPoints(int i, PSEQ_T &s) {
      while(0 <= i-1 && s.ref(i) == s.ref(i-1)) {
        --i;
      }
      return i;
    }
    
    int multiPointsLen(int i, PSEQ_T &s) {
      int j = i;
      while(j+1 < s.size() && s.ref(i) == s.ref(j)) {
        ++j;
      }
      return j - i;
    }
    
    POINT_T rectPush(POINT_T p, POINT_T rt) {
      return POINT_T::right_top(p, rt) - rt;
    }
    
    void undiplicate(PSET_T &set, PSEQ_T &x, PSEQ_T &y) {
      TYPE prev, bias;
      prev = x.ref(0).getX();
      bias = 0;
      for(int i=1; i<set.size(); ++i) {
        if(x.ref(i).getX() + bias <= prev) {
          bias += grid_size;
        }
        x.ref(i).setX(x.ref(i).getX() + bias);
        prev = x.ref(i).getX();
      }
      prev = y.ref(0).getY();
      bias = 0;
      for(int i=1; i<set.size(); ++i) {
        if(y.ref(i).getY() + bias <= prev) {
          bias += grid_size;
        }
        y.ref(i).setY(y.ref(i).getY() + bias);
        prev = y.ref(i).getY();
      }
    }
    
    int prevX(int ix, int iy, PSEQ_T &x, PSEQ_T &y) {
      TYPE t = y.ref(iy).getY();
      TYPE u = y.ref(iy).getX();
      do {
        if(ix <= 0) {
          return -1;
        }
      } while(t < x.ref(--ix).getY()
              || (t == x.ref(ix).getY() && u < x.ref(ix).getX()));
      return ix;
    }
    
    int prevY(int ix, int iy, PSEQ_T &x, PSEQ_T &y) {
      TYPE t = x.ref(ix).getX();
      TYPE u = x.ref(ix).getY();
      do {
        if(iy <= 0) {
          return -1;
        }
      } while(t < y.ref(--iy).getX()
              || (t == y.ref(iy).getX() && u < y.ref(iy).getY()));
      return iy;
    }
    
    void listCountRects(PSET_T &set, PSEQ_T &x, PSEQ_T &y) {
      grid<int> cr(set.size());
      RDIC_T rd(set.size());
      int pi, pj, v;
      for(int i=0; i<set.size(); ++i) {
        for(int j=0; j<set.size(); ++j) {
          if(i != rewindMultiPoints(i, x)) {
            continue;
          }
          if(j != rewindMultiPoints(j, y)) {
            continue;
          }
          pi = prevX(i,j,x,y);
          pi = rewindMultiPoints(pi, x);
          v = pi < 0 ? 0 : cr.ref(pi, j);
          rd[v+1].push_back(Point<int>(i,j));
          cr.ref(i, j) = v+1;
        }
      }
      calcTranslateTable(set, x, y, rd);
    }
    
    void calcTranslateTable(PSET_T &set, PSEQ_T &x, PSEQ_T &y, RDIC_T &rd) {
      grid<POINT_T> pt(set.size()), tt(set.size());
      grid<char> dir(set.size());
      int i, j, pi, pj;
      bool multix, multiy;
      TYPE mxlen, mylen;
      POINT_T rt, art, brt, a, b, pa, pb, ta, tb;
      for(RDIC_T::iterator u = rd.begin(); u != rd.end(); ++u) {
        for(RDIC_T::value_type::iterator v = u->begin(); v != u->end(); ++v) {
          i = v->getX();
          j = v->getY();
          i = rewindMultiPoints(i, x);
          j = rewindMultiPoints(j, y);
          pi = prevX(i,j,x,y);
          pj = prevY(i,j,x,y);
          pi = rewindMultiPoints(pi, x);
          pj = rewindMultiPoints(pj, y);
          switch(checkPointPair(i,j,x,y)) {
            case SINGLE : {
              art = POINT_T::right_top(pi < 0 ? POINT_T(0,0) : x.ref(pi),
                                       pj < 0 ? POINT_T(0,0) : y.ref(pj));
              a = x.ref(i).trans(pi < 0 ? POINT_T(0,0) : x.ref(pi),
                                 pj < 0 ? POINT_T(0,0) : y.ref(pj),
                                 grid_size);
              tt.ref(i,j) = ((pi < 0 || pj < 0) ? POINT_T(0,0) : tt.ref(pi,pj))
                          + rectPush(x.ref(i) + a + rectMultiPoints(multiPointsLen(i,x) , grid_size), art);
              pt.ref(i,j) = ((pi < 0 || pj < 0) ? POINT_T(0,0) : tt.ref(pi,pj))
                          + a - art;
              dir.ref(i,j) = RIGHT_TOP;
            } break;
            case DOUBLE : {
              art = POINT_T::right_top(x.ref(pi), y.ref(j));
              a = x.ref(i).trans(x.ref(pi), y.ref(j), grid_size);
              pa = (pi < 0 ? POINT_T(0,0) : tt.ref(pi,j)) + a - art;
              ta = rectPush(x.ref(i) + a + rectMultiPoints(multiPointsLen(i,x), grid_size), art);
              
              brt = POINT_T::right_top(x.ref(i), y.ref(pj));
              b = y.ref(j).trans(x.ref(i), y.ref(pj), grid_size);
              pb = (pj < 0 ? POINT_T(0,0) : tt.ref(i,pj)) + b - brt;
              tb = rectPush(y.ref(j) + b + rectMultiPoints(multiPointsLen(j,y), grid_size), brt);
              
              if(ta.length() <= tb.length()) {
                tt.ref(i,j) = ((pi < 0) ? POINT_T(0,0) : tt.ref(pi,j)) + ta;
                pt.ref(i,j) = pa;
                dir.ref(i,j) = RIGHT;
              } else {
                tt.ref(i,j) = ((pj < 0) ? POINT_T(0,0) : tt.ref(i,pj)) + tb;
                pt.ref(i,j) = pb;
                dir.ref(i,j) = TOP;
              }
            } break;
          }
        }
      }
      applyLayout(set,x,y,pt,dir);
    }
    
    void applyLayout(PSET_T &set, PSEQ_T &x, PSEQ_T &y, grid<POINT_T> &tt, grid<char> &dir) {
      std::list<int> ap;
      bool multix, multiy, istop, isright;
      int ix = set.size()-1, iy = set.size()-1, px = 0, py = 0;
      POINT_T trans;
      PSET_T tmp(set);
      while(ix >= 0 && iy >= 0) {
        multix = checkMultiPoints(ix, x);
        multiy = checkMultiPoints(iy, y);
        ix = rewindMultiPoints(ix, x);
        iy = rewindMultiPoints(iy, y);
        if(!dir.ref(ix,iy)) {
          std::cerr << "APPLY FAILEDED at " << ix << " " << iy << std::endl;
          std::cerr << x.ref(px) << " " << y.ref(py) << std::endl;
          std::cerr << tt.ref(ix, iy) << " : " << x[ix] << " " << y[iy] << std::endl;
          return;
        }
        istop = dir.ref(ix,iy) == TOP;
        isright = dir.ref(ix,iy) == RIGHT;
        trans = tt.ref(ix, iy);
        px = prevX(ix,iy,x,y);
        py = prevY(ix,iy,x,y);
        px = rewindMultiPoints(px, x);
        py = rewindMultiPoints(py, y);
        switch(checkPointPair(ix,iy,x,y)) {
          case SINGLE : {
            if(multix) {
              int it = ix, rb = 0, tb = 0, rc = 0, tc = 0, l = multiPointsLen(it,x);
              POINT_T rect = rectMultiPoints<TYPE>(l, grid_size);
              while((it - ix <= l) && x.ref(ix) == x.ref(it))
              {
                tmp[x[it]] += trans + POINT_T(rc * grid_size, tc * grid_size);
                ap.push_front(x[it]);
                if(tc == tb) {
                  if(rc == 0) {
                    ++rb;
                    ++tb;
                    rc = rb;
                    tc = 0;
                  } else {
                    --rc;
                  }
                } else {
                  ++tc;
                }
                ++it;
              }
            } else {
              ap.push_front(x[ix]);
              tmp[*ap.begin()] += trans;
            }
            ix=px;
            iy=py;
          } break;
          case DOUBLE : {
            if(dir.ref(ix,iy) != TOP) {
              if(multix) {
                int it = ix, rb = 0, tb = 0, rc = 0, tc = 0, l = multiPointsLen(it,x);
                POINT_T rect = rectMultiPoints<TYPE>(l, grid_size);
                while((it - ix <= l) && x.ref(ix) == x.ref(it))
                {
                  tmp[x[it]] += trans + POINT_T(rc * grid_size, tc * grid_size);
                  ap.push_front(x[it]);
                  if(tc == tb) {
                    if(rc == 0) {
                      ++rb;
                      ++tb;
                      rc = rb;
                      tc = 0;
                    } else {
                      --rc;
                    }
                  } else {
                    ++tc;
                  }
                  ++it;
                }
              } else {
                ap.push_front(x[ix]);
                tmp[*ap.begin()] += trans;
              }
              ix=px;
            } else {
              if(multiy) {
                int it = iy, rb = 0, tb = 0, rc = 0, tc = 0, l = multiPointsLen(it,y);
                POINT_T rect = rectMultiPoints<TYPE>(l, grid_size);
                while((it - iy <= l) && y.ref(iy) == y.ref(it))
                {
                  tmp[y[it]] += trans + POINT_T(rc * grid_size, tc * grid_size);
                  ap.push_front(y[it]);
                  if(tc == tb) {
                    if(rc == 0) {
                      ++rb; ++tb;
                      rc = rb;
                      tc = 0;
                    } else {
                      --rc;
                    }
                  } else {
                    ++tc;
                  }
                  ++it;
                }
              } else {
                ap.push_front(y[iy]);
                tmp[*ap.begin()] += trans;
              }
              iy=py;
            }
          } break;
        }
      }
      set = tmp;
    }

    void foldLayout(PSET_T &set) {
      PSEQ_T x(set, true), y(set, false);
      for(int i=1; i<x.size(); ++i)
      {
        if( (x.ref(i).getX() == x.ref(i-1).getX() + grid_size) &&
            (x.ref(i).getY() > x.ref(i-1).getY()) )
        {
          for(int j=i; j<x.size(); ++j) {
            x.ref(j) += POINT_T(-grid_size, 0);
          }
        }
      }
      for(int i=1; i<y.size(); ++i)
      {
        if( (y.ref(i).getY() == y.ref(i-1).getY() + grid_size) &&
            (y.ref(i).getX() > y.ref(i-1).getX()) )
        {
          for(int j=i; j<y.size(); ++j) {
            y.ref(j) += POINT_T(0, -grid_size);
          }
        }
      }
    }
  public:
    GridLayoutMan(int grid_size) : grid_size(grid_size) {}
    ~GridLayoutMan() {}
    
    PSET_T &match(PSET_T &p) {
      PSEQ_T sortx(p, true), sorty(p, false);
      listCountRects(p, sortx, sorty);
      //foldLayout(p);
      return p;
    }
    
    bool checkMatch(PSET_T &p) {
      for(int i=0; i<p.size(); ++i) {
        if(p[i].getX() % grid_size != 0 || p[i].getY() % grid_size != 0) {
          return false;
        }
      }
      return true;
    }
  };
}

template<typename TYPE>
gridlayout::PointSetProxy<TYPE>::PointSetProxy() : set_p(__null) {
}

template<typename TYPE>
gridlayout::PointSetProxy<TYPE>::PointSetProxy(const char *filename) : set_p(__null) {
  std::ifstream fin(filename);
  if(fin.is_open()) {
    this->set_p = new gridlayout::PointSet<TYPE>(fin);
  }
}

template<typename TYPE>
gridlayout::PointSetProxy<TYPE>::PointSetProxy(std::istream &pIn) : set_p(__null) {
  this->set_p = new gridlayout::PointSet<TYPE>(pIn);
}

template<typename TYPE>
gridlayout::PointSetProxy<TYPE>::PointSetProxy(const TYPE *pset, const size_t length) : set_p(__null) {
  this->set_p = new gridlayout::PointSet<TYPE>(pset, length);
}

template<typename TYPE>
gridlayout::PointSetProxy<TYPE>::~PointSetProxy() {
  if(set_p) delete set_p;
}

template<typename TYPE>
size_t gridlayout::PointSetProxy<TYPE>::length() {
  return set_p->size();
}

template<typename TYPE>
void gridlayout::PointSetProxy<TYPE>::print() {
  std::cout << *set_p << std::endl;
}

template<typename TYPE>
void gridlayout::PointSetProxy<TYPE>::print(std::ostream &sOut) {
  sOut << *set_p << std::endl;
}

template<typename TYPE>
void gridlayout::PointSetProxy<TYPE>::print(TYPE *out_p, const size_t maxlen) {
  int k=0;
  for(typename std::vector<Point<TYPE> >::iterator i = set_p->begin();
      i != set_p->end() || k<maxlen;
      ++i, k+=2)
  {
    out_p[k] = i->getX();
    out_p[k+1] = i->getY();
  }
}

template<typename TYPE>
int gridlayout::GridLayout(PointSetProxy<TYPE> *pointset, const TYPE gridsize)
{
  if(!pointset->set_p) {
    return -1;
  }
  GridLayoutMan<TYPE> gl(gridsize);
  gl.match(*(pointset->set_p));
  if(!pointset->set_p->checkIndependent()) {
    std::cerr << "NOT INDEPENDENT MATCH!" << std::endl;
  }
  return 0;
}

template class gridlayout::PointSetProxy<int>;
template class gridlayout::PointSetProxy<float>;
template class gridlayout::PointSetProxy<double>;

template int gridlayout::GridLayout(PointSetProxy<int> *pointset, const int gridsize);
template int gridlayout::GridLayout(PointSetProxy<double> *pointset, const double gridsize);

