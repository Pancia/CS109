#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "xless.h"
#include "xpair.h"

template <typename Key, typename Value, class Less=xless<Key>>
class listmap {
   public:
      using key_type = Key;
      using mapped_type = Value;
      using value_type = xpair<const key_type, mapped_type>;
   private:
      Less less;
      struct node;
      struct link {
         node* next{};
         node* prev{};
         link(node* next, node* prev):
             next(next), prev(prev){}
      };
      struct node: link {
         value_type value{};
         node(node* next, node* prev, const value_type&);
      };
      node* anchor() { return static_cast<node*> (&anchor_); }
      link anchor_ {anchor(), anchor()};
   public:
      class iterator;
      listmap(){};
      listmap(const listmap&);
      listmap& operator =(const listmap&);
      ~listmap();
      iterator insert(const value_type&);
      iterator find(const key_type&);
      iterator erase(iterator position);
      iterator begin() { return anchor()->next; }
      iterator end() { return anchor(); }
      bool empty() { return begin() == end(); }
      void print();
      void print(const mapped_type&);
};

template <typename Key, typename Value, class Less>
class listmap<Key,Value,Less>::iterator {
   private:
      friend class listmap<Key,Value>;
      listmap<Key,Value,Less>::node* where {nullptr};
      iterator(node* where): where(where) {};
   public:
      iterator() {}
      value_type& operator *();
      value_type* operator ->();
      iterator& operator ++(); //++itor
      iterator& operator --(); //--itor
      void erase();
      bool operator ==(const iterator&) const;
      bool operator !=(const iterator&) const;
};

#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node(node* next, node* prev,
                                     const value_type& value):
            link(next, prev), value(value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);
}

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert(const value_type& pair) {
    TRACE ('l', &pair << "->" << pair);
    node *new_node = new node(nullptr, nullptr, pair);
    if (empty()) {
        begin().where->next = new_node;
        begin().where->prev = nullptr;
        new_node->next = end().where;
    } else {
        for (auto it = begin(); it != end(); ++it) {
            if (pair.first == it.where->value.first) {
                it.where->value.second = pair.second;
                return it;
            }
        }
        begin().where->prev = new_node;
        new_node->next = begin().where;
        anchor_.next = new_node;
    }
    return end();
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find(const key_type& that) {
    TRACE ('l', that);
    if (empty()) {
        return end();
    }
    for (auto it = begin(); it != end(); ++it) {
        if (it.where->value.first == that) {
            return it;
        }
    }

    return end();
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::print() {
    if (empty()) {
        return;
    }

    for (auto it : *this) {
        cout << it << endl;
    }
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::print(const mapped_type& val) {
    if (empty()) {
        return;
    }

    for (auto it : *this) {
        if (val == it.second) {
            cout << it << endl;
        }
    }
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase(iterator position) {
    TRACE ('l', &*position);
    iterator it;
    for (it = begin(); it != end(); ++it) {
        if (it == position) {
            if (it == begin() && it.where->next == end().where) {
                node *tmp = it.where;
                anchor_.next = end().where;
                delete tmp;
            } else if (it == begin()) {
                node *tmp = begin().where;
                anchor_.next->next->prev = nullptr;
                anchor_.next = begin().where->next;
                delete tmp;
            } else if (it.where->next == end().where) {
                node *tmp = it.where;
                tmp->prev->next = end().where;
                tmp->prev = nullptr;
                delete tmp;
            } else {
                it.erase();
            }
            break;
        }
    }
    return it;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase() {
    node *tmp = where;
    where->next->prev = where->prev;
    where->prev->next = where->next;
    delete tmp;
}

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
    TRACE('l', where);
    return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
    TRACE('l', where);
    return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
    TRACE ('l', where);
    where = where->next;
    return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
    TRACE('l', where);
    where = where->prev;
    return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
(const iterator& that) const {
    return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
(const iterator& that) const {
    return this->where != that.where;
}

#endif
