#ifndef UTILITY_INL_
#define UTILITY_INL_
#pragma once

#include "Utility.h"

// -- linked list -------------------------------------------------------------

// -- linked list node --
template <typename T>
Util::Linked_List_Node<T>::Linked_List_Node(const T _d) {
  prev = next = nullptr;
  data = _d;
}

// -- linked list --

template <typename T>
Util::Linked_List<T>::Linked_List() {
  header = last = nullptr;
  size = 0;
}

template <typename T>
Util::Linked_List<T>::~Linked_List() {
  while ( true ) {
    if ( !header ) return;
    Linked_List_Node<T*>* t = header->next;
    delete header;
    header = t;
  }
}

template <typename T>
Util::Linked_List_Node<T*>* Util::Linked_List<T>::R_Head()
 { return header; }

template <typename T>
Util::Linked_List_Node<T*>* Util::Linked_List<T>::R_Tail()
  { return last; }

template <typename T>
void Util::Linked_List<T>::Push(T* data) {
  if ( size++ == 0 ) {
    header = last = new Linked_List_Node<T*>(data);
    return;
  }
  last->next = new Linked_List_Node<T*>(data);
  auto* _l = last;
  last = last->next;
  last->prev = _l;
}

template <typename T>
T* Util::Linked_List<T>::Pop() {
  if ( size == 0 ) return nullptr;
  --size;
  if ( size != 0 )
    last->prev->next = nullptr;
  Linked_List_Node<T*>* _l = last;
  last = last->prev;
  return _l->data;
}

template <typename T>
int Util::Linked_List<T>::R_Size() const { return size; }

// --- miscel

template <typename T>
T Util::Str_To_Type(std::string x) {
  std::stringstream x;
  x >> o;
  T i;
  x << i;
  return i;
}

template<typename T, typename ...Args>
void Util::Reconstruct(T& obj, Args&&... args) {
  static_assert(!std::has_virtual_destructor<T>::value,
                "obj can't have virtual destructor\n");
  obj.~T();
  new (&obj) T(std::forward<Args>(args)...);
}

#endif