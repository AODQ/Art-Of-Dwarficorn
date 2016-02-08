/* (c) CiggBit. All rights reserved.
 * See license.txt for more information
 */
 
#ifndef __UTILITY_H_
#define __UTILITY_H_
#pragma once

#include "Vector.h"
#include <SDL_Opengl.h>

#include <random>
#include <string>

// general utility purposed things
namespace Util {
  extern std::random_device rand;
  extern std::mt19937 rgen;
}

namespace Util {
  extern const float E,
                     Log10E,
                     Log2E,
                     Pi,
                     Tau,
                     Max_float,
                     Min_float,
                     Epsilon;

  float R_Rand(float bot, float top);

  template <typename T>
  T R_Max(T x, T y);
  template <typename T>
  T R_Min(T x, T y);

  bool Find_Substr(std::string&, std::string);
  template <typename T>
  T Str_To_Type(std::string x);

  void Delete_Image(GLuint);
  
  AOD::Vector Rotate_Point(AOD::Vector origin, AOD::Vector point, float ang);

  template <typename T, typename... Args>
  void Reconstruct(T& obj, Args&&... args);

  inline float To_Rad(float x) {
    return x * (Util::Pi/180.0f);
  }

  template <typename T>
  class Linked_List_Node {
  public:
    T data;
    Linked_List_Node<T>* next, *prev;
    Linked_List_Node(T data);
  };

  template <typename T>
  class Linked_List {
    Linked_List_Node<T*>* header, *last;
    int size;
  public:
    Linked_List();
    ~Linked_List();
    Linked_List_Node<T*>* R_Head();
    Linked_List_Node<T*>* R_Tail();
    void Push(T* data);
    T* Pop();
    int R_Size() const;
  };

  using LL = long long int;
  class Time {
    LL hours, minutes, seconds, milliseconds;
    void Refresh_Value();
  public:
    Time(LL ms = 0, LL min = 0, LL sec = 0, LL h = 0);
    void Set_Time(LL ms = 0, LL min = 0,
                  LL sec = 0, LL h = 0);
    void Add_Time(LL ms = 0, LL min = 0,
                  LL sec = 0, LL h = 0);

    std::string R_Time_Str();
    LL R_Hours() const;
    LL R_Minutes() const;
    LL R_Seconds() const;
    LL R_Milliseconds() const;
  };
}

// asserts expression X. If !X, outputs Y and returns Z
#define ret_assert2(X, Y, Z) if ( !(X) ) {\
  AOD::Output(Y);\
  /*__debugbreak();*/\
  return Z; }
// asserts expression X. If !X, outputs Y and returns void()
#define ret_assert(X, Y) if ( !(X) ) {\
  AOD::Output(Y);\
  __debugbreak();\
  return; }

// asserts expression. if expression, writes to err file and
// program exits.
void ex_assert(bool expression, std::string err_msg);

#include "Utility.inl"

#endif