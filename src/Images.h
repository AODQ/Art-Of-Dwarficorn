/* (c) CiggBit. All rights reserved.
 * See license.txt for more information
 */
 
#ifndef Images_h_
#define Images_h_
#pragma once

#include "AOD.h"

#include <SDL_opengl.h>
#include <vector>

// contains information of image
class Sheet_Container {
public:
  GLuint texture;
  int width, height;
  Sheet_Container();
  Sheet_Container(GLuint,int,int);
  Sheet_Container(char*);
};

// contains location of sprite inside a sheet
class Sheet_Rect : public Sheet_Container {
public:
  AOD::Vector ul, lr;
  Sheet_Rect(); // nill
  Sheet_Rect(const Sheet_Container&, AOD::Vector ul, AOD::Vector lr);
};

class Animation {
public:
  enum class Type {
    Linear, Zigzag, Repeat
  };
  using SRect_Vec = std::vector<Sheet_Rect>;
private:
  Sheet_Container image;
  SRect_Vec frames;
  float end;
  Type type;
public:
  //Animation(const Sheet_Container&, float ms_end, Type = Type::Linear);
  Animation(SRect_Vec, const Sheet_Container&,
            float ms_end, Type = Type::Linear);
  
  /*void Add_Frame(const Sheet_Rect&);
  void Set_Sheet_Container(const Sheet_Container&);
  void Set_MS_Per_Frame(float ms);*/
  // returns true if ended

  inline const Sheet_Container& R_Image() const { return image; }
  inline int R_Size() const { return frames.size(); }
  inline Type R_Type() const { return type; }
  inline int R_End()   const { return end; }
  int R_Frame(float ms) const;
  Sheet_Rect* R_Frame_SR(float ms);
};

class Background {
public:
  Sheet_Container* image;

  bool vertical = 0, horizontal = 0, repeats = 0;
  float speed_x = 0,// background speed is defined by following:
        speed_y = 0;// If  parralax, then 1:1 ratio from map units w/ player movement
  bool parralax = 0;// If !parralax, then 1 unit moved per 1 frame
  int layer = 0;
};

struct Anim_Handler {
public:
  union Type {
    Animation* animation;
    Sheet_Rect* sheet;
  };
  bool animation;
  bool dir;
  bool done;
  Type type;
  float timer;
  Anim_Handler();
  Anim_Handler(Animation* anim);
  Anim_Handler(Sheet_Rect* sheet);

  void Reset();
  void Set(Sheet_Rect* sheet);
  void Set(Animation* anim, int siz = 1);
  void Update();
  Sheet_Rect* R_SR();
  float R_Time_Ratio() const;
};

class PSheet {
public:
  PSheet();
  Sheet_Container img;
  // ----- legs ------
  enum class Anim_Legs {
    stand_idle, landing, punch_front, punch_back, to_crouch, crouch_idle,
    walk_turn, crouch_turn, run_forward, run_turn, run_back, walk,
    crouch_walk, jump_hi, jump_crouch, jump_strafe1, jump_strafe2,
    slide1, slide2, airbone_idle, swim, dash_vertical, dash_horiz1,
    dash_horiz2, walljump1, walljump2
  };
  Animation* legs_stand_idle, *legs_landing, *legs_punch_front,
            *legs_punch_back, *legs_to_crouch, *legs_crouch_idle,
            *legs_walk_turn, *legs_crouch_turn, *legs_run_forward,
            *legs_run_turn, *legs_run_back, *legs_walk,
            *legs_crouch_walk, *legs_jump_hi, *legs_jump_crouch,
            *legs_jump_strafe[2], *legs_slide[2], *legs_airbone_idle,
            *legs_swim, *legs_dash_vertical, *legs_dash_horiz[2],
            *legs_walljump[2];
  Sheet_Rect legs_turn;
  std::map<Anim_Legs, Animation*> leg_anims;
  // ----- deaths -----
  enum { DEATH_B, DEATH_F };
  Animation* death_start[2]; // (includes death_in_air)
  int death_in_air;
  Animation* death_end[2];

  // ----- hands -----
  enum class Static_Hand {
    twowep_left_f, twowep_right_f, twowep_left_b, twowep_right_b,
    onewep_f, onewep_b,
    alarm_right_b, alarm_right_f, alarm_left_b, alarm_left_f,
    guard_left_f, guard_right_f,
    guard_left_b, guard_right_b,
    slide_1_f, slide_1_b, slide_2_f, slide_2_b,
    size
  };
  enum class Anim_Hands {
    hold_wep_two, hold_wep_one, alarmed, guard, slide_1, slide_2,
    run, walk, punch1, punch2, punch3, jump, strafe1, strafe2,
    backflip, frontflip
  };
  Sheet_Rect hands_static[20];
  Sheet_Rect hands_static_punch_f[6],
             hands_static_punch_b[6];
  Animation* hands_run_f,        *hands_run_b,      *hands_walk_f,
           * hands_walk_b,       *hands_punch_f[3], *hands_punch_b[3],
           * hands_jump_f[2],    *hands_jump_left_b, *hands_jump_left_f,
           * hands_jump_right_f, *hands_jump_right_b,
           * hands_strafe1_f,    *hands_strafe1_b,
           * hands_strafe2_f,    *hands_strafe2_b,
           * hands_backflip_f,   *hands_backflip_b,
           * hands_frontflip_f,  *hands_frontflip_b;

  // ----- head/body -----
  Sheet_Rect head_up, head_mid, head_down;
  Sheet_Rect body_4px_left,  body_2px_left,  body_center,
             body_4px_right, body_2px_right, body_walk;
};

Sheet_Container Load_Image(const char* fil);
Sheet_Container Load_Image(std::string fil);

#endif