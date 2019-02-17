#include "utility/timer.h"
#include "game/playfield.h"

#include <iostream>
#include <set>

namespace {

// DAS settings
const int64_t kAutoRepeatInitialDelay = 300; // milliseconds
const int64_t kAutoRepeatSubsequentDelay = 50; // milliseconds

const std::set<Playfield::Controls> kAutoRepeatControls = {
  Playfield::Controls::Left,
  Playfield::Controls::Right,
  Playfield::Controls::Up,
  Playfield::Controls::Down
};

}  // namespace

using namespace utility;

class Qix {
 public:
  using RepeatFunc = std::function<void()>;

  Qix() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    if (TTF_Init() != 0) {
      std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
      exit(-1);
    }
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    playfield_ = std::make_shared<Playfield>();
  }

  ~Qix() {
    playfield_.reset();
    DetachController(gamecontroller_index_);
    SDL_Quit();
    TTF_Quit();
  }

  void DisplayJoystickInfo(int index) {
    auto js = SDL_JoystickOpen(index);

    if (nullptr == js) {
      std::cout << "Unknown joystick - unable to find information: " << SDL_GetError() << std::endl;
      return;
    }
    char guid_str[1024];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(js), guid_str, sizeof(guid_str));

    const auto name = SDL_JoystickName(js);

    std::cout << guid_str << ", " << name << " - not found in database" << std::endl;;

    SDL_JoystickClose(js);
  }

  void AttachController(int index) {
    if (nullptr != game_controller_ || SDL_IsGameController(index) == 0) {
      if (nullptr == game_controller_) {
        DisplayJoystickInfo(index);
      }
      return;
    }
    game_controller_ = SDL_GameControllerOpen(index);
    if (nullptr == game_controller_) {
      std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    gamecontroller_index_ = index;
    gamecontroller_name_ = SDL_GameControllerNameForIndex(gamecontroller_index_);
    std::cout << "Game controller attached: " << gamecontroller_name_ << std::endl;
  }

  void DetachController(int index) {
    if (nullptr == game_controller_ || index != gamecontroller_index_) {
      return;
    }
    std::cout << "Game controller detached: " << gamecontroller_name_ << std::endl;
    SDL_GameControllerClose(game_controller_);
    game_controller_ = nullptr;
    gamecontroller_index_ = -1;
    gamecontroller_name_ = "";
  }

  Playfield::Controls TranslateKeyboardCommands(const SDL_Event& event) const {
    if (event.key.repeat > 0) {
      return Playfield::Controls::None;
    }
    const auto code = event.key.keysym.scancode;

    if (SDL_SCANCODE_LEFT == code) {
      std::cout << "L" << std::endl;
      return Playfield::Controls::Left;
    } else if (SDL_SCANCODE_RIGHT == code) {
      std::cout << "R" << std::endl;
      return Playfield::Controls::Right;
    } if (SDL_SCANCODE_DOWN == code) {
      std::cout << "D" << std::endl;
      return Playfield::Controls::Down;
    } else if (SDL_SCANCODE_UP == code) {
      std::cout << "U" << std::endl;
      return Playfield::Controls::Up;
    } else if (SDL_SCANCODE_LSHIFT == code) {
      return Playfield::Controls::Fast;
    } else if (SDL_SCANCODE_LCTRL == code) {
      return Playfield::Controls::Slow;
    } else if (SDL_SCANCODE_N == code) {
      return Playfield::Controls::Start;
    } else if (SDL_SCANCODE_P == code || SDL_SCANCODE_F1 == code) {
      return Playfield::Controls::Pause;
    } else if (SDL_SCANCODE_Q == code) {
      return Playfield::Controls::Quit;
    }

    return Playfield::Controls::None;
  }

  Playfield::Controls TranslateControllerCommands(const SDL_Event& event) const {
    if (event.cbutton.which != gamecontroller_index_) {
      return Playfield::Controls::None;
    }
    switch (event.cbutton.button) {
      case SDL_CONTROLLER_BUTTON_A:
        return Playfield::Controls::Fast;
      case SDL_CONTROLLER_BUTTON_B:
        return Playfield::Controls::Slow;
      case SDL_CONTROLLER_BUTTON_BACK:
        return Playfield::Controls::Pause;
      case SDL_CONTROLLER_BUTTON_START:
        return Playfield::Controls::Start;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return Playfield::Controls::Slow;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return Playfield::Controls::Fast;
      case SDL_CONTROLLER_BUTTON_DPAD_UP:
        std::cout << "U" << std::endl;
        return Playfield::Controls::Up;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        std::cout << "D" << std::endl;
        return Playfield::Controls::Down;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        std::cout << "L" << std::endl;
        return Playfield::Controls::Left;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        std::cout << "R" << std::endl;
        return Playfield::Controls::Right;
    }
    return Playfield::Controls::None;
  }

  template <Playfield::Controls control>
  std::pair<Playfield::Controls, RepeatFunc> Repeatable(int& repeat_count, int64_t& time_since_last_auto_repeat) {
    repeat_count = 0;
    time_since_last_auto_repeat = kAutoRepeatInitialDelay;
    return std::make_pair(control, [&playfield = playfield_]() { playfield->GameControl(control); });
  }

  void Play() {
    bool quit = false;
    DeltaTimer delta_timer;
    int repeat_count = 0;
    int64_t time_since_last_auto_repeat = 0;
    int64_t auto_repeat_threshold = kAutoRepeatInitialDelay;
    RepeatFunc function_to_repeat;
    auto active_control = Playfield::Controls::None;
    SDL_Event event;

    while (!quit) {
      auto control = Playfield::Controls::None;

      while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
          quit = true;
          break;
        }
        switch (event.type) {
          case SDL_KEYDOWN:
            control = TranslateKeyboardCommands(event);
            break;
          case SDL_CONTROLLERBUTTONDOWN:
            control = TranslateControllerCommands(event);
            break;
          case SDL_KEYUP:
            std::cout << "UP ";
            if (TranslateKeyboardCommands(event) == active_control) {
              active_control = Playfield::Controls::None;
            }
            break;
          case SDL_CONTROLLERBUTTONUP:
            std::cout << "UP ";
            if (TranslateControllerCommands(event) == active_control) {
              active_control = Playfield::Controls::None;
            }
            break;
          case SDL_JOYDEVICEADDED:
            if (SDL_IsGameController(event.jbutton.which) == 0) {
              DisplayJoystickInfo(event.jbutton.which);
              break;
            }
          case SDL_CONTROLLERDEVICEADDED:
            AttachController(event.cbutton.which);
            break;
          case SDL_JOYDEVICEREMOVED:
            event.cbutton.which = event.jbutton.which;
          case SDL_CONTROLLERDEVICEREMOVED:
            DetachController(event.cbutton.which);
            break;
        }
      }
      switch (control) {
        case Playfield::Controls::None:
          break;
        case Playfield::Controls::Left:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Playfield::Controls::Left>(repeat_count, time_since_last_auto_repeat);
          break;
        case Playfield::Controls::Right:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Playfield::Controls::Right>(repeat_count, time_since_last_auto_repeat);
          break;
        case Playfield::Controls::Up:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Playfield::Controls::Up>(repeat_count, time_since_last_auto_repeat);
          break;
        case Playfield::Controls::Down:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Playfield::Controls::Down>(repeat_count, time_since_last_auto_repeat);
          break;
        case Playfield::Controls::Start:
          playfield_->NewGame();
          active_control = control;
          break;
        case Playfield::Controls::Pause:
          playfield_->Pause();
          active_control = control;
          break;
        case Playfield::Controls::Quit:
          quit = true;
          break;
        default:
          playfield_->GameControl(control);
          active_control = control;
          break;
      }
      if (kAutoRepeatControls.count(active_control) && (time_in_ms() - time_since_last_auto_repeat) >= auto_repeat_threshold) {
        function_to_repeat();
        auto_repeat_threshold = (0 == repeat_count) ? kAutoRepeatInitialDelay : kAutoRepeatSubsequentDelay;
        repeat_count++;
        time_since_last_auto_repeat = time_in_ms();
      }
      playfield_->Update(delta_timer.GetDelta());
    }
  }

 private:
  int gamecontroller_index_ = -1;
  std::string gamecontroller_name_;
  SDL_GameController* game_controller_ = nullptr;
  std::shared_ptr<Playfield> playfield_ = nullptr;
};

int main(int, char *[]) {
  Qix qix;

  qix.Play();

  return 0;
}
