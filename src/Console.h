#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SDL.h>
#include "Object.h"
#include "Text.h"

#include <vector>


namespace AOD {
  // debug_in means debug messages generated from AOD will be left in otherwise
  // they will be left out.
  namespace Console {
    extern const int TYPE_NONE,
                     TYPE_DEBUG_IN,
                     TYPE_DEBUG_OUT;
  }
}

namespace AOD_Engine {
  namespace Console {
    extern int console_type;
    extern std::vector<AOD::Text*> console_text;
    extern AOD::Text* input;
    extern AOD::Text* input_after;
    extern AOD::Text* input_sig;
    extern int console_input_cursor,
               console_input_select_len;
    extern AOD::Object* cursor,
                      * background;
    extern unsigned char console_message_count;
    extern GLuint console_image;
    // the below will be processed as debug messages when Refresh is called
    extern std::vector<std::string> to_console;
    void Construct();
    void Deconstruct();
    void Refresh();
    extern SDL_Keycode key;
    extern int console_history;
  }
}

namespace AOD {
  namespace Console {
    extern bool console_open;
    void Set_Open_Console_Key(SDL_Keycode key);
    void Set_Console_History(int history_limit);
  }
}

#endif