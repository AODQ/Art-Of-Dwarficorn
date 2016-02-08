/* (c) CiggBit. All rights reserved.
 * See license.txt for more information
 */
#include "stdafx.h"
#include "AOD.h"

#include "Images.h"
#include "Utility.h"

#include <fstream>
#include <mutex>

#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <tuple>


Sheet_Container::Sheet_Container(){
  texture = 0;
  width = height = 0;
}

Sheet_Container::Sheet_Container(GLuint t, int w, int h) {
  texture = t;
  width = w;
  height = h;
}

Sheet_Container::Sheet_Container(char* image_name) {
  auto z = Load_Image(image_name);
  texture = z.texture;
  width   = z.width;
  height  = z.height;
}

// ---- animations ------------------------------------------------------------

/*Animation::Animation(const Sheet_Container& sc, float ms, Type t) {
  type = t;
  image = sc;
  end = ms;
  dir = 1;
}*/

Animation::Animation(SRect_Vec rec, const Sheet_Container& sc,
                     float ms, Type t) {
  type = t;
  image = sc;
  end = ms;
  frames = rec;
}

int Animation::R_Frame(float ms) const {
  float fs = frames.size()-1;
  float f  = ms/end * fs;
  if ( f < 0  ) f = 0;
  if ( f > fs ) f = fs;
  return f;
}

Sheet_Rect* Animation::R_Frame_SR(float ms) {
  return &frames[R_Frame(ms)];
}


// ---- sheet rect ------------------------------------------------------------

Sheet_Rect::Sheet_Rect() {}
Sheet_Rect::Sheet_Rect(const Sheet_Container& sc, AOD::Vector tul,
                       AOD::Vector tlr) : Sheet_Container(sc) {
  ul = tul;
  lr = tlr;
  width  = abs(ul.x - lr.x);
  height = abs(ul.y - lr.y);
  ul.x /= sc.width; lr.y /= sc.height;
  lr.x /= sc.width; ul.y /= sc.height;
  float ty = lr.y;
  lr.y = 1 - ul.y;
  ul.y = 1 - ty;
}


// ---- utility functions -----------------------------------------------------

static std::mutex load_image_mutex;
Sheet_Container Load_Image(const char* fil) {
  std::lock_guard<std::mutex> load_image_lock(load_image_mutex);

  ret_assert2(std::ifstream(fil).good(), "File " + std::string(fil) +
              " not found", Sheet_Container());
  ILuint IL_ID;
  GLuint GL_ID;
  int width, height;

  ilGenImages(1, &IL_ID);
  ilBindImage(IL_ID);
  if ( ilLoadImage( fil ) == IL_TRUE ) {
    ILinfo ImageInfo;
    iluGetImageInfo(&ImageInfo);
    if ( ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT )
      iluFlipImage();

      if ( !ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) ) {
        auto t = iluErrorString(ilGetError());
        AOD_Engine::Debug_Output(t);
        return 0;
      }
      glBindTexture(GL_TEXTURE_2D, 0);
      glGenTextures(1, &GL_ID);
      glBindTexture(GL_TEXTURE_2D, GL_ID);
      if ( !glIsTexture(GL_ID) ) {
        AOD::Output("Error generating GL texture");
        return Sheet_Container();
      }
      // set texture clamping method
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      // set texture interpolation method
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
      width  = ilGetInteger(IL_IMAGE_WIDTH);
      height = ilGetInteger(IL_IMAGE_HEIGHT);

      // texture specs
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    } else {
      auto t = ilGetError();
      AOD_Engine::Debug_Output("Error loading " + std::string(fil) + ": " +
        iluErrorString(t) + "(" + std::to_string(ilGetError()) + ')');
      return 0;
    }

    ilDeleteImages(1, &IL_ID);
    return Sheet_Container(GL_ID, width, height);
}

Sheet_Container Load_Image(std::string fil) {
  return Load_Image(fil.c_str());
}

Anim_Handler::Anim_Handler() {
  Reset();
  type.sheet = nullptr;
}
Anim_Handler::Anim_Handler(Animation* anim) {
  Reset();
  animation = 1;
  type.animation = anim;
}
Anim_Handler::Anim_Handler(Sheet_Rect* sheet) {
  Reset();
  animation = 0;
  type.sheet = sheet;
}

void Anim_Handler::Reset() {
  dir = 1;
  timer = 0;
  done = 0;
}

void Anim_Handler::Set(Sheet_Rect* sheet) {
  if ( animation || type.sheet != sheet ) {
    animation = 0;
    type.sheet = sheet;
    Reset();
  }
}

void Anim_Handler::Set(Animation* anim, int siz) {
  if ( !animation || type.animation != anim ) {
    animation = 1;
    type.animation = anim;
    Reset();
  }
}

void Anim_Handler::Update() {
  if ( animation ) {
    timer += AOD::R_MS() * (dir? 1 : -1);
    if ( timer < 0 ) {
      if ( type.animation->R_Type() == Animation::Type::Zigzag ) {
        timer = type.animation->R_End()/type.animation->R_Size();
        dir = 1;
      } else {
        done = 1;
        if ( type.animation->R_Type() == Animation::Type::Repeat )
          timer = type.animation->R_End();
      }
    } else if ( timer > type.animation->R_End() ) {
      if ( type.animation->R_Type() == Animation::Type::Zigzag ) {
        timer = type.animation->R_End() - 
                  type.animation->R_End()/type.animation->R_Size();
        dir = 0;
      } else {
        done = 1;
        if ( type.animation->R_Type() == Animation::Type::Repeat )
          timer = 0;
      }
    }
  }
}

Sheet_Rect* Anim_Handler::R_SR() {
  if ( animation ) {
    if ( type.animation )
      return type.animation->R_Frame_SR(timer);
    return nullptr;
  } else {
    if ( type.sheet )
      return type.sheet;
    return nullptr;
  }
}

float Anim_Handler::R_Time_Ratio() const {
  return timer/(float)(type.animation->R_End());
}

// ---- psheet ----
PSheet::PSheet() {
  
}