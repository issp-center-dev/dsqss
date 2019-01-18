
#ifndef LINK_H
#define LINK_H

#include <cstdio>

//######################################################################

template <class C>
class Linked;
template <class C>
class RingIterator;
template <class C>
class Ring;
template <class C>
class Pool;

//######################################################################
/*
inline void abort() {
  printf("ERROR: Aborting...\n");
  exit(0);
};
*/
//######################################################################

template <class C>
class Linked : public C {
private:
  Linked<C>* p;
  Linked<C>* n;

public:
  void init() {
    C::init();
    p = this;
    n = this;
  };

  Linked() { init(); };

  ~Linked(){};

  Linked<C>& prev() { return *p; };

  Linked<C>& next() { return *n; };

  void set_prev(Linked<C>& x) { p = &x; };

  void set_next(Linked<C>& x) { n = &x; };

  void insert_after(Linked<C>& x);

  void insert_before(Linked<C>& x);

  void remove();

  void dump();
};

//######################################################################

// The class C must have the following member functions
//   C& next() ... returns the reference to the next item
//   C& prev() ... returns the reference to the previous item
//   C& remove() ... connect prev() and next() and detatch
//                   itself from the linked list

template <class C>
class RingIterator {
private:
public:
  C* org;
  C* cur;

  void init(C& x) {
    org = &x;
    cur = org;
  };

  void init(Ring<C>& r) {
    org = &r.root();
    cur = org;
  };

  RingIterator(){};

  RingIterator(const RingIterator& p) {
    org = p.org;
    cur = p.cur;
  };

  RingIterator(C& x) { init(x); };

  RingIterator(Ring<C>& r) { init(r); };

  bool atOrigin() { return cur == org; };

  RingIterator<C> operator+(int i) {
    RingIterator<C> p;
    p.org = org;
    p.cur = &(cur->next());
    return p;
  };

  RingIterator<C>& operator++() {
    cur = &(cur->next());
    return *this;
  };

  RingIterator<C>& operator++(int) {
    cur = &(cur->next());
    return *this;
  };

  RingIterator<C>& operator--() {
    cur = &(cur->prev());
    return *this;
  };

  RingIterator<C>& operator--(int) {
    cur = &(cur->prev());
    return *this;
  };

  C* operator->() { return cur; };
  C& operator*() { return *cur; };
  void operator=(const C* p) { cur = p; };
  bool operator==(const C* p) { return cur == p; };
  bool operator!=(const C* p) { return cur != p; };
};

//######################################################################

// The class C must have the following member functions
//   C::C() ... A newly created object must form a ring by itself
//              (Its next (prev) object is itself.)
//   C& C::next() ... returns the reference to the next item
//   C& C::prev() ... returns the reference to the previous item
//   C& C::remove() ... connect prev() and next() and detatch
//                   itself from the linked list

template <class C>
class Ring {
  friend class RingIterator<C>;

private:
  //  C ROOT;
public:
  C ROOT;
  C& head() { return ROOT.next(); };
  C& tail() { return ROOT.prev(); };

  typedef RingIterator<C> iterator;

  Ring(){};

  ~Ring(){};

  bool empty() { return (&(ROOT.next()) == &ROOT); };

  C& root() { return ROOT; };

  C& first() { return ROOT.next(); };

  void add_head(C& x) { ROOT.insert_after(x); };

  void add_tail(C& x) { ROOT.insert_before(x); };

  C& remove_head() {
    C& x = head();
    x.remove();
    return x;
  };

  C& remove_tail() {
    C& x = tail();
    x.remove();
    return x;
  };

  void push(C& x) { add_head(x); };

  C& pop() { return remove_head(); };

  int count();

  RingIterator<C> sort_min();

  void move_to_head(RingIterator<C>);

  void dump() {
    RingIterator<C> p(*this);
    printf("Root: ");
    printf(" %p -> %p -> %p\n", &(p->prev()), &(*p), &(p->next()));
    while (!(++p).atOrigin())
      p->dump();
  }
};

//######################################################################

template <class C>
class Pool : public Ring<C> {
private:
  int size_max;
  int size_min;
  int size;

public:
  Pool() : Ring<C>() { size = 0; };

  ~Pool();

  void init(int N);

  void push(C& x) {
    size++;
    x.init();
    Ring<C>::push(x);
  };

  C& pop() {
    if (size == 0) {
      printf("Pool> ERROR. Attempt to extract from an empty pool.\n");
      exit(0);
    }

    size--;
    if (size < size_min) size_min = size;
    C& x = Ring<C>::pop();
    //  x.init();
    return x;
  };

  int number_of_used_elements() { return size_max - size_min; };
  void set_n_of_used_elements(int s_used) { size_min = size_max - s_used; };
};

//######################################################################
//  Member Functions
//######################################################################

template <class C>
inline void Linked<C>::insert_after(Linked<C>& x) {
  // *p -> *this -> x -> *n
  x.set_prev(*this);
  x.set_next(this->next());
  next().set_prev(x);
  set_next(x);
}

template <class C>
inline void Linked<C>::insert_before(Linked<C>& x) {
  x.set_prev(prev());
  x.set_next(*this);  // `this` points the ROOT instance
  prev().set_next(x);
  set_prev(x);
}

template <class C>
inline void Linked<C>::remove() {
  //if (ALERT) {
  //  printf("\nLinked::remove> BEFORE\n");
  //  printf("Linked::remove> *this:\n");
  //  this->dump();
  //  printf("Linked::remove> prev():\n");
  //  prev().dump();
  //  printf("Linked::remove> next():\n");
  //  next().dump();
  //}
  prev().set_next(next());
  next().set_prev(prev());
  //if (ALERT) {
  //  printf("\nLinked::remove> AFTER\n");
  //  printf("Linked::remove> *this:\n");
  //  this->dump();
  //  printf("Linked::remove> prev():\n");
  //  prev().dump();
  //  printf("Linked::remove> next():\n");
  //  next().dump();
  //}
}

template <class C>
inline void Linked<C>::dump() {
  printf(" %p ->%p ->%p : ", &prev(), this, &next());
  C::dump();
}

//######################################################################

template <class C>
inline int Ring<C>::count() {
  int c = 0;
  RingIterator<C> p(*this);
  while (!(++p).atOrigin())
    c++;
  return c;
}

//######################################################################
// C requre operation <

template <class C>
RingIterator<C> Ring<C>::sort_min() {
  RingIterator<C> p1(*this);
  RingIterator<C> p2(*this);
  ++p1;
  ++p1;
  ++p2;
  while (!p1.atOrigin()) {
    if (((*p1) < (*p2)) || ((*p1).V_x == (*p2).V_x)) {
      p2 = p1;
      ++p1;
      move_to_head(p2);
    } else {
      ++p1;
    }
  }
  return p2;
}

template <class C>
void Ring<C>::move_to_head(RingIterator<C> it) {
  C& Cmin = (*it);
  it->remove();
  add_head(Cmin);
}

//######################################################################

template <class C>
inline void Pool<C>::init(int N) {
  // +++ edit sakakura +++
  //if ( ! Ring<C>::empty() ) {
  //  printf("Pool: ERROR. Attempt to initialize the pool twice.\n");
  //  exit(0);
  //}
  // +++ edit sakakura +++

  for (int i = 0; i < N; i++) {
    C& x = *(new C);
    this->add_tail(x);
  }
  size     = N;
  size_max = N;
  size_min = N;
}

//======================================================================

template <class C>
inline Pool<C>::~Pool() {
  //  printf("*** Destroying Pool\n");
  while (!Ring<C>::empty()) {
    C& x = pop();
    delete &x;
  }
}

#endif
