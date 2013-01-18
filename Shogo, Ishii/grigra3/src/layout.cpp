#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <list>

#define DEFINE_STREAM_PRINT(T) \
  template<typename TYPE>\
  std::ostream &operator <<(std::ostream &out, T<TYPE> &p) { \
    p.print(out); return out; \
  }

#define RIGHT 1
#define TOP  2
#define RIGHT_TOP 3
#define MULTI_RIGHT 4
#define MULTI_TOP 5

template<typename TYPE>
class Point {
  TYPE x, y;
public:
  Point() : x(0), y(0) {}
  Point(TYPE x, TYPE y) : x(x), y(y) {}
  Point(const Point &p) : x(p.x), y(p.y) {}
  ~Point() {}
#define GETTER(T,N,V) T get ## N () const { return V ; }
#define SETTER(T,N,V) void set ## N (T V) { this-> V = V; }
  GETTER(TYPE,X,x) GETTER(TYPE,Y,y)
  SETTER(TYPE,X,x) SETTER(TYPE,Y,y)
#undef GETTER
#undef SETTER
#define UV(A) \
  TYPE u##A = (std::abs(t.A) % grid) * (t.A > 0 ? -1 : 1); \
  TYPE v##A = (grid - std::abs(u##A)) * (t.A > 0 ? 1 : -1);
  Point trans(Point q, int grid, int dir) {
    Point t(*this - q), r, tmp; UV(x) UV(y)
    r = Point(ux,uy);
//    if(dir == TOP || dir == RIGHT_TOP) {
      r.y = uy < vy ? vy : uy;
//    } else {
//      r.y = std::abs(uy) < std::abs(vy) ? uy : vy;
//    }
//    if(dir == RIGHT || dir == RIGHT_TOP) {
      r.x = ux < vx ? vx : ux;
//    } else {
//      r.x = std::abs(ux) < std::abs(vx) ? ux : vx;
//    }
    //std::cerr << "TRANS " << *this << " / " << r << std::endl;
    return r;
  }
#undef UV
  TYPE sum() const { return x+y; }
  TYPE length() const { return abs(x)+abs(y); }
  bool operator ==(Point &p) { return (x == p.x) && (y == p.y); }
  Point &operator =(Point p) { x=p.x; y=p.y; return *this; }
  Point operator +(Point &p) { Point q(x + p.x, y + p.y); return q; }
  Point &operator +=(Point p) { x += p.x; y += p.y; return *this; }
  Point &operator -=(Point p) { x -= p.x; y -= p.y; return *this; }
  Point operator -(Point &p) { Point q(x - p.x, y - p.y); return q; }
  void print(std::ostream &out) { out << x << " " << y; }
};
DEFINE_STREAM_PRINT(Point)

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

template<typename TYPE>
class PointSet : public std::vector<Point<TYPE> > {
  typedef Point<TYPE> POINT_T;
  typedef typename std::vector<POINT_T>::iterator iterator;
public:
#define BEGIN std::vector<POINT_T>::begin()
#define END std::vector<POINT_T>::end()
#define SIZE std::vector<POINT_T>::size()
#define AT(I) std::vector<POINT_T>::at(I)
  PointSet() : std::vector<POINT_T>() {}
  ~PointSet() {}
  PointSet(std::istream &in) : std::vector<POINT_T>() {
    TYPE x, y; in >> x, in >> y;
    while(!in.eof()) push_back(POINT_T(x,y)), in >> x, in >> y;
  }
  PointSet(PointSet &set) : std::vector<POINT_T>(set.size()) {
    for(int i=0; i<set.size(); ++i) { std::vector<POINT_T>::at(i) = set[i]; }
  }
  PointSet(int size) : std::vector<POINT_T>(size) {}
  void print(std::ostream &out) {
    for(iterator i = BEGIN; i != END; i++) i->print(out), out << std::endl;
  }
  bool checkIndependent() {
    bool check = true;
    for(iterator i = BEGIN; i != END; i++)
      for(iterator j = i; j != END; j++)
        if(!(i==j) && (*i==*j)) {
          check = false;
          // std::cerr << *i << " : " << *j << std::endl;
        }
    return check;
  }
  PointSet &operator=(PointSet &set) {
    for(int i=0; i<SIZE; ++i) AT(i) = set[i];
    return *this;
  }
#undef BEGIN
#undef END
#undef SIZE
#undef AT
};
DEFINE_STREAM_PRINT(PointSet)

template<typename TYPE>
class PointSequence : public std::vector<int> {
  typedef typename std::vector<int>::iterator iterator;
#define DEFINE_COMPARE(A,B) \
  class compare_##A { \
    PointSet<TYPE> &set; \
  public: \
    compare_##A (PointSet<TYPE> &set) : set(set) {} \
    bool operator ()(const TYPE &a, const TYPE &b) const { \
      if(set[a].get##A() == set[b].get##A()) \
        return set[a].get##B() < set[b].get##B(); \
      return set[a].get##A() < set[b].get##A(); \
    } \
  };
  DEFINE_COMPARE(X,Y)
  DEFINE_COMPARE(Y,X)
#undef DEFINE_COMPARE
#define BEGIN std::vector<int>::begin()
#define END std::vector<int>::end()
  PointSet<TYPE> &set;
public:
  ~PointSequence() {}
  PointSequence(PointSet<TYPE> &set, bool sortx)
  : std::vector<int>(set.size()), set(set) {
    int j=0;
    for(iterator i=BEGIN; i!=END; ++i) { *i = j++; }
    if(sortx) std::sort(BEGIN, END, compare_X(set));
    else std::sort(BEGIN, END, compare_Y(set));
  }
  Point<TYPE> &ref(int i) { return set.at(at(i)); }
  void print(std::ostream &out) {
    for(iterator i=BEGIN; i!=END; ++i) out << *i << " : " << set[*i] << std::endl;
  }
#undef BEGIN
#undef END
};
DEFINE_STREAM_PRINT(PointSequence)

template <typename TYPE>
class grid : public std::vector<TYPE> {
  typedef typename std::vector<TYPE>::iterator iterator;
  int grid_size;
public:
  grid() : std::vector<TYPE>(), grid_size(0) {}
  grid(int size) : std::vector<TYPE>(size*size), grid_size(size) {}
  TYPE &ref(int x, int y) { return std::vector<TYPE>::at(x + y * grid_size); }
  void print(std::ostream &out) {
    int j=0;
    for(iterator i=std::vector<TYPE>::begin(); i!=std::vector<TYPE>::end(); i++) {
      out << "  <" << *i << ">";
      if(++j % grid_size == 0) out << std::endl;
    }
  }
};
DEFINE_STREAM_PRINT(grid)

template <typename TYPE>
class GridLayout {
  typedef Point<TYPE> POINT_T;
  typedef PointSet<TYPE> PSET_T;
  typedef PointSequence<TYPE> PSEQ_T;
  typedef std::vector< std::list< Point<int> > > RDIC_T;
  int grid_size;
  int checkPointPair(int i, int j, PSEQ_T &x, PSEQ_T &y) {
    if(x[i] == y[j]) return 1;
    POINT_T &p = x.ref(i), &q = y.ref(j);
    if(p.getX() >= q.getX() && p.getY() <= q.getY()) return 2;
    return 0;
  }
  bool checkMultiPoints(int i, PSEQ_T &s) {
    if(i+1 < s.size() && s.ref(i) == s.ref(i+1)) return true;
    if(0 <= i-1 && s.ref(i) == s.ref(i-1)) return true;
    return false;
  }
  int rewindMultiPoints(int i, PSEQ_T &s) {
    while(0 <= i-1 && s.ref(i) == s.ref(i-1)) --i;
    return i;
  }
  int multiPointsLen(int i, PSEQ_T &s) {
    int j = i;
    while(j+1 < s.size() && s.ref(i) == s.ref(j)) ++j;
    return j - i;
  }
#define UNDIPLICATE_AXIS(A,a) \
  prev = a.ref(0).get##A(), bias = 0; \
  for(int i=1; i<set.size(); ++i) { \
    if(a.ref(i).get##A() + bias <= prev) { \
      bias += grid_size; \
    } \
    a.ref(i).set##A(a.ref(i).get##A() + bias); \
    prev = a.ref(i).get##A(); \
  }
  void undiplicate(PSET_T &set, PSEQ_T &x, PSEQ_T &y) {
    TYPE prev, bias;
    UNDIPLICATE_AXIS(X,x)
    UNDIPLICATE_AXIS(Y,y)
  }
#undef UNDIPLICATE_AXIS
#define DEFINE_PREV(A1,a1,A2,a2) \
  int prev##A1(int ix, int iy, PSEQ_T &x, PSEQ_T &y) { \
    TYPE t = a2.ref(i##a2).get##A2(); \
    TYPE u = a2.ref(i##a2).get##A1(); \
    if(a1.ref(i##a1).get##A1() == a2.ref(i##a2).get##A1() && \
       a1.ref(i##a1).get##A2() < a2.ref(i##a2).get##A2()) { \
      return 1 + i##a1; \
    } \
    do { \
      if(i##a1 <= 0) return -1; \
    } while(t < a1.ref(--i##a1).get##A2() || \
            (t == a1.ref(i##a1).get##A2() && u < a1.ref(i##a1).get##A1())); \
    return i##a1; \
  }
  DEFINE_PREV(X,x,Y,y)
  DEFINE_PREV(Y,y,X,x)
#undef DEFINE_PREV
#define CASE_TEMPLATE(ID,CODE) \
  case ID : { \
    px = prevX(i,j,x,y), py = prevY(i,j,x,y); \
    CODE \
  } break;
#define PCOUNT(V,T,X,Y) V = (T) ? 0 : cr.ref(X,Y)
#define SETCOUNT(V) cr.ref(i,j) = (V)+1; rd[V].push_back(Point<int>(i,j)); rdcnt[V]+=1;
#define EACH_CELL(X,Y) for(int X=0; X<set.size(); ++X) for(int Y=0; Y<set.size(); ++Y)
  void listCountRects(PSET_T &set, PSEQ_T &x, PSEQ_T &y) {
    grid<int> cr(set.size());
    RDIC_T rd(set.size());
    std::vector<int> rdcnt(set.size(), 0);
    int px, py, a, b;
    EACH_CELL(i,j) {
      switch(checkPointPair(i,j,x,y)) {
        CASE_TEMPLATE(1,
          PCOUNT(a, px<0 || py<0, px, py);
          SETCOUNT(a);)
        CASE_TEMPLATE(2,
          PCOUNT(a, px<0, px, j);
          SETCOUNT(a);)
      }
    }
    calcTranslateTable(set, x, y, rd);
  }
#undef CASE_TEMPLATE
#undef PCOUNT
#undef SETCOUNT
#undef EACH_CELL
#define CASE_TEMPLATE(ID,CODE) \
  case ID : { \
    px = prevX(i,j,x,y), py = prevY(i,j,x,y); \
    CODE \
  } break;
#define CALC_AXIS(A,IA,X,IX,IY,LOGIC,D) \
  X = (LOGIC) ? POINT_T() : tt.ref(IX,IY); \
  pre##X = (LOGIC) ? POINT_T() : ((dir.ref(IX,IY)!=2) ? x.ref(IX) : y.ref(IY)); \
  d##X = A.ref(IA).trans(pre##X, grid_size, D); \
  X##len = d##X.length() + ((LOGIC) ? 0 : sum.ref(IX,IY)) + (multi##A ? m##A##len : 0)
#define SET_TABLE(T,S,D) tt.ref(i,j)=T, sum.ref(i,j)=S,dir.ref(i,j)=D
#define GRID(T, N) grid<T> N(set.size());
  void calcTranslateTable(PSET_T &set, PSEQ_T &x, PSEQ_T &y, RDIC_T &rd) {
    GRID(POINT_T, tt) GRID(TYPE, sum) GRID(int, dir)
    bool multix, multiy;
    int i, j, px,py;
    TYPE alen,blen, mxlen, mylen;
    POINT_T a,b,prea,preb,da,db;
    for(RDIC_T::iterator u = rd.begin(); u != rd.end(); ++u) {
      for(RDIC_T::value_type::iterator v = u->begin(); v != u->end(); ++v) {
        i = v->getX(), j = v->getY();
        multix = checkMultiPoints(i, x);
        multiy = checkMultiPoints(j, y);
        if(multix) {
          i = rewindMultiPoints(i, x);
          mxlen = sumMultiPointsDistance(multiPointsLen(i, x), grid_size);
        }
        if(multiy) {
          j = rewindMultiPoints(j, y);
          mylen = sumMultiPointsDistance(multiPointsLen(j, y), grid_size);
        }
        switch(checkPointPair(i,j,x,y)) {
          CASE_TEMPLATE(1,
            CALC_AXIS(x,i,a,px,py,px<0||py<0,RIGHT_TOP);
            SET_TABLE(da,alen,RIGHT_TOP);)
          CASE_TEMPLATE(2,
            CALC_AXIS(x,i,a,px,j,px<0,RIGHT);
            CALC_AXIS(y,j,b,i,py,py<0,TOP);
            if(alen < blen) {
              SET_TABLE(da,alen,RIGHT);
            } else {
              SET_TABLE(db,blen,TOP);
            })
        }
      }
    }
    applyLayout(set,x,y,tt,dir);
  }
#undef CASE_TEMPLATE
#undef CALC_AXIS
#undef SET_TABLE
#undef GRID
#define CASE_TEMPLATE(ID,CODE) \
  case ID : { \
    CODE \
  } break;
#define PUSH(A) ap.push_front(&tmp[A[i##A]])
#define MULTI_PUSH(A) { \
    if(multi##A) { \
      std::cerr << "Multi-Points Found! " << ix << " " << iy << std::endl; \
      int it = i##A, rb = 0, tb = 0, rc = 0, tc = 0, l = multiPointsLen(it,A); \
      std::cerr << "size : " << l << std::endl; \
      POINT_T rect = rectMultiPoints<TYPE>(l, grid_size); \
      while((it - i##A <= l) && A.ref(i##A) == A.ref(it)) { \
        std::cerr << rc << " " << tc << std::endl; \
        tmp[A[it]] += POINT_T(rc * grid_size, tc * grid_size); \
        ap.push_front(&tmp[A[it]]); \
        if(tc == tb) { \
          if(rc == 0) { \
            ++rb; \
            ++tb; \
            rc = rb; \
            tc = 0; \
          } else { \
            --rc; \
          } \
        } else { \
          ++tc; \
        } \
        ++it; \
      } \
      typename std::list<POINT_T*>::iterator itr = ap.begin(); \
      while(i##A <= --it) { \
        **itr -= rect; \
        ++itr; \
      } \
      std::cerr << "Rect : " << rect << std::endl; \
      trans += rect; \
    } else { \
      PUSH(A); \
    } \
  }
  void applyLayout(PSET_T &set, PSEQ_T &x, PSEQ_T &y, grid<POINT_T> &tt, grid<int> &dir) {
    std::list<POINT_T*> ap;
    bool multix, multiy;
    int ix = set.size()-1, iy = set.size()-1, px = 0, py = 0;
    POINT_T trans;
    PSET_T tmp(set);
    std::cerr << tt << std::endl;
    std::cerr << dir << std::endl;
    while(ix >= 0 && iy >= 0) {
      std::cerr << "Apply! : " << ix << " " << iy << std::endl;
      // multiple-point block
      multix = checkMultiPoints(ix, x);
      multiy = checkMultiPoints(iy, y);
      if(multix) {
        std::cerr << "multi-x! " << ix << " " << iy << std::endl;
        ix = rewindMultiPoints(ix, x);
      }
      if(multiy) {
        std::cerr << "multi-y! " << ix << " " << iy << std::endl;
        iy = rewindMultiPoints(iy, y);
        std::cerr << "to " << ix << " " << iy << std::endl;
      }
      if(!dir.ref(ix,iy)) {
        std::cerr << "Failed : " << "prev / " << px << " " << py << " i / " << ix << " " << iy << std::endl;
        return;
      }
      trans = tt.ref(ix, iy);
      px = prevX(ix,iy,x,y), py = prevY(ix,iy,x,y);
      std::cerr << "Prev : " << px << " " << py << std::endl;
      switch(checkPointPair(ix,iy,x,y)) {
        CASE_TEMPLATE(1,
          MULTI_PUSH(x); ix=px; iy=py;)
        CASE_TEMPLATE(2,
          if(dir.ref(ix,iy)!=TOP) { MULTI_PUSH(x) ix=px; }
          else { MULTI_PUSH(y) iy=py; })
      }
      for(typename std::list<POINT_T*>::iterator i=ap.begin(); i!=ap.end(); ++i)
      { *(*i) += trans; }
    }
    set = tmp;
  }
#undef CASE_TEMPLATE
#undef PUSH
#undef MULTI_PUSH
public:
  GridLayout(int grid_size) : grid_size(grid_size) {}
  ~GridLayout() {}
  PSET_T &match(PSET_T &p) {
    PSEQ_T sortx(p, true), sorty(p, false);
    std::cerr << "SORT X:" << std::endl << sortx << std::endl;
    std::cerr << "SORT Y:" << std::endl << sorty << std::endl;
    //undiplicate(p, sortx, sorty);
    listCountRects(p, sortx, sorty);
    //if(!p.checkIndependent()) {
    //  std::cerr << "NOT INDEPENDENT, UNDIPLICATING..." << std::endl;
    //  {
    //    PSEQ_T sortx(p, true), sorty(p, false);
    //    undiplicate(p, sortx, sorty);
    //  }
    //}
    return p;
  }
  bool checkMatch(PSET_T &p) {
    for(int i=0; i<p.size(); ++i)
      if(p[i].getX() % grid_size != 0 || p[i].getY() % grid_size != 0)
        return false;
    return true;
  }
};

template<typename TYPE>
bool ApplyGridLayout(int grid, std::istream &in, std::ostream &out) {
  GridLayout<TYPE> gridlayout(grid);
  PointSet<TYPE> pointset(in);
  //if(!pointset.checkIndependent()) return false;
  //if(gridlayout.checkMatch(pointset)) return false;
  std::cerr << pointset << std::endl;
  gridlayout.match(pointset);
  out << pointset;
  if(!pointset.checkIndependent()) {
    std::cerr << "NOT INDEPENDENT." << std::endl;
    return false;
  }
  if(!gridlayout.checkMatch(pointset)) {
    std::cerr << "NOT MATCHING." << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "USAGE [layout.exe grid_size/int]" <<std::endl;
    return 0;
  }
  if(!ApplyGridLayout<int>(atoi(argv[1]), std::cin, std::cout)) {
    std::cerr << "GridLayout Failed." << std::endl;
  }
  return 0;
}
