#include "stdafx.h"

#include "Utility.h"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <sstream>
#include <string>


const float Util::E         =  2.718282f,
            Util::Log10E    =  0.4342945f,
            Util::Log2E     =  1.442695f,
            Util::Pi        =  3.141593f,
            Util::Tau       =  6.283185f,
            Util::Max_float =  3.402823E+38f,
            Util::Min_float = -3.402823E+38f,
            Util::Epsilon   =  0.000001f;

std::random_device Util::rand;
std::mt19937 Util::rgen(Util::rand());

float Util::R_Rand(float bot, float top) {
  std::uniform_real_distribution<> rn(bot, top);
  return rn(Util::rgen);
}


template <typename T>
T Util::R_Max(T x, T y) { return x > y ? x : y; }

template <typename T>
T Util::R_Min(T x, T y) { return x < y ? x : y; }


bool Util::Find_Substr(std::string& out, std::string in) {
  if ( in == "" ) return false;
  int it = 0, hit_count = 0;
  for ( auto i : out ) {
    if ( in[it++] == i ) ++hit_count;
    else { it = 0; hit_count = 0; }
    if ( hit_count == in.size() ) break;
  }
  if ( hit_count == in.size() ) return true;

  return false;
}

void Util::Delete_Image(GLuint t) {
  
}

AOD::Vector Util::Rotate_Point(AOD::Vector o, AOD::Vector p, float angle) {
  return {
    std::cos(angle)*(p.x-o.x) - std::sin(angle)*(p.y-o.y) + o.x,
    std::sin(angle)*(p.x-o.x) - std::cos(angle)*(p.y-o.y) + o.y
  };
}

void ex_assert(bool statement, std::string err_msg) {
  if ( !statement ) {
    std::ofstream err("__EXIT_ERROR");
    err << "Err: " << err_msg << "\n";
    err.close();
    exit(0);
  }
}



// --- time -------------------------------------------------------------------
using LL = long long int;
Util::Time::Time(LL ms, LL min, LL sec, LL h) {
  Set_Time(ms, min, sec, h);
}
void Util::Time::Refresh_Value() {
  seconds += milliseconds/1000;
  milliseconds %= 1000;
  minutes += seconds/60;
  seconds %= 60;
  hours   += minutes/60;
  minutes %= 60;
}

void Util::Time::Set_Time(LL ms, LL min, LL sec, LL h) {
  milliseconds = ms;
  minutes      = min;
  seconds      = sec;
  hours        = h;
}

void Util::Time::Add_Time(LL ms, LL min, LL sec, LL h) {
  milliseconds += ms;
  minutes      += min;
  seconds      += sec;
  hours        += h;
  Refresh_Value();
}

std::string Util::Time::R_Time_Str() {
  return std::to_string(hours) + ":"   + std::to_string(minutes) + ":" +
         std::to_string(seconds) + "." + std::to_string(milliseconds);
}

LL Util::Time::R_Hours() const {
  return hours;
}

LL Util::Time::R_Minutes() const {
  return minutes;;
}

LL Util::Time::R_Seconds() const {
  return seconds;
}

LL Util::Time::R_Milliseconds() const {
  return milliseconds;
}