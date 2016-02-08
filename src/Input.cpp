#include "stdafx.h"
#include "Input.h"

namespace EngInp = AOD_Engine::Input;
namespace AODInp = AOD::Input;

unsigned char EngInp::mouse;
int EngInp::mouse_x,
    EngInp::mouse_y;


void EngInp::Refresh_Input() {
  AODInp::keys = (Uint8*)SDL_GetKeyboardState(nullptr);
  mouse = SDL_GetMouseState(&mouse_x,&mouse_y);
  // Let AOD handle mouse wheel
}

Uint8* AODInp::keys = nullptr;

bool AODInp::R_LMB() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_LEFT      ); }
bool AODInp::R_RMB() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_RIGHT     ); }
bool AODInp::R_MMB() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_MIDDLE    ); }
//bool AODInp::R_MWU() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_WHEELUP   ); }
//bool AODInp::R_MWD() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_WHEELDOWN ); }
bool AODInp::R_MX1() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_X1        ); }
bool AODInp::R_MX2() { return EngInp::mouse&SDL_BUTTON(SDL_BUTTON_X2        ); }