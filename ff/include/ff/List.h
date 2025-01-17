/**
 * @file List.h
 * @author liyawu
 * @date 2025-01-17 09:41:02
 * @description
 */

#ifndef _FF_LIST_H
#define _FF_LIST_H

#include <ff/ff_config.h>

#include <vector>

NS_FF_BEG

template <typename T>
class List {
 public:
  using VT = std::vector<T*>;

  class iterator {
   public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef size_t size_type;
    typedef size_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;

    inline iterator(const typename VT::iterator& it) noexcept { this->it = it; }
    inline iterator(const iterator& it) noexcept { this->it = it.it; }
    inline T& operator*() { return **it; }
    inline bool operator==(const iterator& o) const noexcept {
      return it == o.it;
    }
    inline bool operator!=(const iterator& o) const noexcept {
      return it != o.it;
    }
    inline bool operator>(const iterator& o) const noexcept {
      return it > o.it;
    }
    inline bool operator>=(const iterator& o) const noexcept {
      return it >= o.it;
    }
    inline bool operator<(const iterator& o) const noexcept {
      return it < o.it;
    }
    inline bool operator<=(const iterator& o) const noexcept {
      return it <= o.it;
    }
    inline iterator& operator++() noexcept {
      ++it;
      return *this;
    }
    inline iterator operator++(int) const noexcept {
      auto oldIt = it;
      ++it;
      return oldIt;
    }
    inline iterator& operator--() noexcept {
      --it;
      return *this;
    }
    inline iterator operator--(int) const noexcept {
      auto oldIt = it;
      --it;
      return oldIt;
    }
    inline iterator& operator+=(difference_type i) noexcept {
      it += i;
      return *this;
    }
    inline iterator& operator-=(difference_type i) noexcept {
      it -= i;
      return *this;
    }
    inline difference_type operator+(const iterator& i) const noexcept {
      return it + i.it;
    }
    inline difference_type operator-(const iterator& i) const noexcept {
      return it - i.it;
    }
    inline iterator operator+(difference_type i) const noexcept {
      return it + i;
    }
    inline iterator operator-(difference_type i) const noexcept {
      return it - i;
    }

   private:
    typename VT::iterator it;
    friend class List<T>;
  };

  class const_iterator {
   public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef size_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;

    inline const_iterator(const typename VT::const_iterator& it) noexcept {
      this->it = it;
    }
    inline const_iterator(const const_iterator& it) noexcept {
      this->it = it.it;
    }
    inline const_iterator(const iterator& it) noexcept { this->it = it.it; }
    inline const T& operator*() const { return **it; }

    inline bool operator==(const const_iterator& o) const noexcept {
      return it == o.it;
    }
    inline bool operator!=(const const_iterator& o) const noexcept {
      return it != o.it;
    }
    inline bool operator>(const const_iterator& o) const noexcept {
      return it > o.it;
    }
    inline bool operator>=(const const_iterator& o) const noexcept {
      return it >= o.it;
    }
    inline bool operator<(const const_iterator& o) const noexcept {
      return it < o.it;
    }
    inline bool operator<=(const const_iterator& o) const noexcept {
      return it <= o.it;
    }
    inline const_iterator& operator++() noexcept {
      ++it;
      return *this;
    }
    inline const_iterator operator++(int) const noexcept {
      auto oldIt = it;
      ++it;
      return oldIt;
    }
    inline const_iterator& operator--() noexcept {
      --it;
      return *this;
    }
    inline const_iterator operator--(int) const noexcept {
      auto oldIt = it;
      --it;
      return oldIt;
    }
    inline const_iterator& operator+=(difference_type i) noexcept {
      it += i;
      return *this;
    }
    inline const_iterator& operator-=(difference_type i) noexcept {
      it -= i;
      return *this;
    }
    inline const_iterator operator+(difference_type i) const noexcept {
      return it + i;
    }
    inline const_iterator operator-(difference_type i) const noexcept {
      return it - i;
    }

   private:
    typename VT::const_iterator it;
    friend class List<T>;
  };

  typedef size_t size_type;
  typedef size_t difference_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;

  inline List() {}
  inline List(const List<T>& l) {
    for (auto& o : l) this->push_back(o);
  }
  inline List(List<T>&& l) { m_d = std::move(l.m_d); }
  template <typename Iterator>
  inline List(Iterator first, Iterator last) {
    std::copy(first, last, std::back_inserter(*this));
  }
  inline ~List() {
    for (auto& o : m_d) delete o;
  }
  inline List& operator=(const List<T>& l) {
    m_d.clear();
    for (auto& o : l) this->push_back(o);
    return *this;
  }
  inline List& operator=(List<T>&& l) {
    m_d = std::move(l.m_d);
    return *this;
  }

  inline size_type size() const { return m_d.size(); }

  inline void push_back(const T& t) {
    auto o = new T(t);
    m_d.push_back(o);
  }

  inline void push_front(const T& t) {
    auto o = new T(t);
    m_d.insert(m_d.begin(), o);
  }

  inline size_type indexOf(const T& t) const {
    for (size_type i = 0; i < m_d.size(); ++i) {
      if (*m_d[i] == t) return i;
    }
    return static_cast<size_type>(-1);
  }

  inline bool empty() const { return m_d.empty(); }

  inline T& at(size_type i) { return *m_d.at(i); }
  inline const T& at(size_type i) const { return *m_d.at(i); }
  inline T at(size_type i, const T& defaultValue) {
    return (i < 0 || i >= size()) ? defaultValue : at(i);
  }
  inline T& operator[](size_type i) { return *m_d[i]; }
  inline const T& operator[](size_type i) const { return *m_d[i]; }
  inline T& front() { return *m_d.front(); }
  inline const T& front() const { return *m_d.front(); }
  inline T& back() { return *m_d.back(); }
  inline const T& back() const { return *m_d.back(); }
  inline void pop_front() {
    auto o = m_d.front();
    delete o;
    m_d.pop_front();
  }
  inline void pop_back() {
    auto o = m_d.back();
    delete o;
    m_d.pop_back();
  }
  inline T take(size_type i) {
    if (i < 0 || i >= size()) return T();
    T t = std::move(at(i));
    erase(i);
    return t;
  }
  inline T take_front() {
    if (empty()) return T();
    T t = std::move(front());
    pop_front();
    return t;
  }
  inline T take_back() {
    if (empty()) return T();
    T t = std::move(back());
    pop_back();
    return t;
  }
  inline void clear() {
    for (auto& o : m_d) delete o;
    m_d.clear();
  }
  inline List& operator<<(const T& t) {
    push_back(t);
    return *this;
  }
  inline bool operator==(const List<T>& l) const {
    if (this == &l) return true;
    if (m_d.size() != l.m_d.size()) return false;
    for (size_type i = 0; i < m_d.size(); ++i) {
      if (*m_d[i] != *l.m_d[i]) return false;
    }
    return true;
  }
  inline bool operator!=(const List<T>& l) const { return !operator==(l); }

  inline iterator begin() { return iterator(m_d.begin()); }
  inline iterator end() { return iterator(m_d.end()); }

  inline const_iterator begin() const { return const_iterator(m_d.begin()); }
  inline const_iterator end() const { return const_iterator(m_d.end()); }

  inline const_iterator cbegin() const { return const_iterator(m_d.cbegin()); }
  inline const_iterator cend() const { return const_iterator(m_d.cend()); }
  inline reverse_iterator rbegin() { return reverse_iterator(end()); }
  inline reverse_iterator rend() { return reverse_iterator(begin()); }
  inline const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  inline const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  inline const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  inline const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(begin());
  }
  inline iterator insert(iterator before, const T& t) {
    m_d.insert(before.it, new T(t));
  }
  inline iterator erase(const_iterator it) {
    auto o = *it.it;
    auto rt = m_d.erase(it.it);
    delete o;
    return rt;
  }

  inline iterator erase(size_type index) {
    if (index < 0 || index >= this->size()) return this->end();
    return erase(this->begin() + index);
  }

  inline bool startsWith(const T& t) const { return !empty() && front() == t; }
  inline bool endsWith(const T& t) const { return !empty() && back() == t; }
  inline List<T> subList(size_type startIndex,
                         size_type length = static_cast<size_type>(-1)) {
    if (startIndex >= size()) return List<T>();

    auto startPos = begin() + startIndex;
    auto endPos =
        (static_cast<size_type>(-1) == length)
            ? end()
            : ((startIndex + length >= size()) ? end() : (startPos + length));
    return List<T>(startPos, endPos);
  }
  inline List<T>& operator+=(const List<T>& l) {
    for (auto& s : l) push_back(s);
    return *this;
  }
  inline List<T> operator+(const List<T>& l) const {
    List<T> newList = *this;
    newList += l;
    return newList;
  }
  inline void swap(size_type i, size_type j) {
    if (i < 0 || i >= size() || j < 0 || j >= size() || i == j) return;
    auto tmp = m_d[i];
    m_d[i] = m_d[j];
    m_d[j] = tmp;
  }

 private:
  VT m_d;
};

NS_FF_END

#endif  // _FF_LIST_H
