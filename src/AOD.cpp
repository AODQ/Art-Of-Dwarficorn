/* (c) CiggBit. All rights reserved.
 * See license.txt for more information
 */
 
#include "stdafx.h"
#include <fstream>
#include <mutex>
#include <SDL.h>

#include "AOD.h"
#include "Camera.h"
#include "Console.h"
#include "Input.h"
#include "Object.h"
#include "Realm.h"
#include "Sounds.h"
#include "Text.h"
#include "Utility.h"

SDL_Window* AOD_Engine::screen = nullptr;
std::vector<GLuint> AOD_Engine::images;

AOD_Engine::Realm* AOD::realm = nullptr;


Uint32 AOD_Engine::ms_dt = 0;

bool AOD_Engine::started = 0;
int  AOD_Engine::start_ticks = 0;
AOD::Text* AOD::fps_display = nullptr;
float AOD_Engine::fps[20] = { 0 };

void AOD::Initialize(int window_width,int window_height, Uint32 fps,
                     char* window_name, char* icon) {
  if ( realm == nullptr ) {
    if ( window_name == "" )
      window_name = "Inspired by the Art Of Dwarficorn";
    if ( icon == "" )
      icon = "AOD_img.bmp";
    realm = new AOD_Engine::Realm(window_width,window_height, window_name, icon);
    AOD_Engine::ms_dt = fps;
  }
  AOD::Camera::Set_Position(Vector(0,0));
  AOD::Camera::Set_Size(Vector((float)window_width,(float)window_height));
}
void AOD::Initialize_Console(bool debug, SDL_Keycode key, std::string cons) {
  if ( debug )
    AOD_Engine::Console::console_type = AOD::Console::TYPE_DEBUG_IN;
  else
    AOD_Engine::Console::console_type = AOD::Console::TYPE_DEBUG_OUT;
  AOD_Engine::Debug_Output("Created new console");
  AOD_Engine::Console::key = key;
  AOD_Engine::Console::Construct();
  //AOD_Engine::Console::console_image = Util::Load_Image(cons.c_str());
}

void AOD::Change_MSDT(Uint32 x) {
  if ( x > 0 )
    AOD_Engine::ms_dt = x;
  else
    AOD_Engine::Debug_Output("Trying to change the MS DeltaTime to a value <= 0");
}

void AOD::Reset() {
  if ( realm != nullptr )
    realm->Reset();
}
void AOD::End() {
  if ( realm != nullptr )
    realm->~Realm();
  SDL_DestroyWindow(AOD_Engine::screen);
  SDL_Quit();
}

std::mutex add_object_mutex;
std::mutex add_text_mutex;

void AOD::Add(AOD::Object* o,int layer) {
  std::lock_guard<std::mutex> add_lock ( add_object_mutex );
  if ( realm != nullptr && o && layer >= 0 )
    realm->__Add(o, layer);
  else {
      if ( o == nullptr )
        AOD_Engine::Debug_Output("Error: Adding nullptr text to realm");
      if ( layer >= 0 )
        AOD_Engine::Debug_Output("Error: Negative layer not allowed for objects");
    }
}

void AOD::Add(Text* t) {
  std::lock_guard<std::mutex> add_lock ( add_text_mutex );
  if ( realm != nullptr && t != nullptr )
    realm->__Add(t);
  else {
    if ( t == nullptr )
      AOD_Engine::Debug_Output("Error: Adding nullptr text to realm");
  }
}

std::mutex rem_mutex;

void AOD::Remove(AOD::Object* o) {
  std::lock_guard<std::mutex> rem_lock ( rem_mutex );
  if ( realm != nullptr )
    realm->__Remove(o);
}
void AOD::Remove(Text* t) {
  if ( realm != nullptr )
    realm->__Remove(t);
}

void AOD::Set_BG_Colour(GLfloat r, GLfloat g, GLfloat b) {
  if ( realm == nullptr ) return;
  realm->Set_BG_Colours(r, g, b);
}

void AOD::Run() {
  if ( realm == nullptr ) return;
  float prev_dt = 0, // DT from previous frame
        curr_dt = 0, // DT for beginning of current frame
        elapsed_dt = 0, // DT elapsed between previous frame and this frame
        accumulated_dt = 0; // DT needing to be processed
  AOD_Engine::started = 1;
  AOD_Engine::start_ticks = SDL_GetTicks();
  SDL_Event _event;
  _event.user.code = 2;
  _event.user.data1 = nullptr;
  _event.user.data2 = nullptr;
  SDL_PushEvent(&_event);

  // so I can set up keys and not have to rely that update is ran first
  SDL_PumpEvents();
  AOD_Engine::Input::Refresh_Input();
  SDL_PumpEvents();
  AOD_Engine::Input::Refresh_Input();

  while ( SDL_PollEvent(&_event) ) {
    switch ( _event.type ) {
      case SDL_QUIT:
        return;
    }
  }
  prev_dt = (float)SDL_GetTicks();
  while ( true ) {
    // refresh time handlers
    curr_dt = (float)SDL_GetTicks();
    elapsed_dt = curr_dt - prev_dt;
    accumulated_dt += elapsed_dt;

    // refresh calculations
    while ( accumulated_dt >= AOD_Engine::ms_dt ) {
      // sdl
      SDL_PumpEvents();
      AOD_Engine::Input::Refresh_Input();
      
      // actual update
      accumulated_dt -= AOD_Engine::ms_dt;
      AOD::realm->Update();

      std::string tex;
      std::string to_handle;
      bool alnum;
      char* chptr = nullptr;

      namespace ECons = AOD_Engine::Console;


      while ( SDL_PollEvent(&_event) );
    }

    { // refresh screen
      float _FPS = 0;
      for ( int i = 0; i != 19; ++ i ) {
        AOD_Engine::fps[i+1] = AOD_Engine::fps[i];
        _FPS += AOD_Engine::fps[i+1];
      }
      AOD_Engine::fps[0] = elapsed_dt;
      _FPS += AOD_Engine::fps[0];

      if ( fps_display != nullptr ) {
        fps_display->Set_String(std::to_string(int(20000/_FPS)) + " FPS");
      }

      AOD_Engine::Console::Refresh();
      realm->Render(); // render the screen
    }

    { // sleep until temp dt reaches ms_dt
      float temp_dt = accumulated_dt;
      temp_dt = float(SDL_GetTicks()) - curr_dt;
      while ( temp_dt < AOD_Engine::ms_dt ) {
        SDL_PumpEvents();
        temp_dt = float(SDL_GetTicks()) - curr_dt;
      }
    }

    // set current frame timemark
    prev_dt = curr_dt;
  }
}

float AOD::R_MS() { return (float)AOD_Engine::ms_dt; }
float AOD::To_MS(float x) { return (x*AOD_Engine::ms_dt)/1000; }

void D_Output(std::string out) {
  std::ofstream fil("DEBUG.txt", std::ios::app);
  fil << out << '\n';
  AOD_Engine::Console::to_console.push_back(out);
  fil.close();
}

static std::mutex output_lock;

void AOD::Output(std::string out) {
  std::lock_guard<std::mutex> lock(output_lock);
  D_Output(out);
}
void AOD_Engine::Debug_Output(std::string out) {
  if ( AOD_Engine::Console::console_type == AOD::Console::TYPE_DEBUG_IN )
    D_Output(out);
}
