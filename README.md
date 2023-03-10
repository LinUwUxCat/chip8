# CHIP-8
## A CHIP-8 emulator/interpreter written in C++ with SDL2
This is a CHIP-8 emulator with Dear ImGui setup on top of SDL for the rendering, and as such a menu is provided to customize a few things within the emulator itself.
## Roadmap
- ~~Add proper settings~~ and remove the Debug menu
- Add a config file to save settings
## QnA
### Why won't [game] run / Why is [game] not working properly?
Only the CHIP-8 instructions are implemented, meaning [anything specific to other versions](https://games.gulrak.net/cadmium/chip8-opcode-table.html) may not work properly.\
If you think/know your game should run on CHIP-8, then try switching to the "CHIP-8" preset in settings.
### Why does the option for the XO-CHIP exist if it doesn't do anything?
I have not taken the time to properly implement it yet!
### Why is the key input not how it was on CHIP-8 systems?
Because i find having to release keys for them to register is very confusing as a user, so i decided to send the input whenever the key is just pressed.\
 And yes, i am aware that this results in the [test suite](https://github.com/Timendus/chip8-test-suite) saying "not released" in the keypad test.\
  Additionally, the input system (intentionally) only supports one input at once.
### Why can't i remap my keys?
Again, this isn't implemented yet. default key placement on a QWERTY keyboard is as follows:
```
Keys     |  QWERTY   |  AZERTY
1 2 3 C  |  1 2 3 4  |  & é " '
4 5 6 D  |  Q W E R  |  A Z E R
7 8 9 E  |  A S D F  |  Q S D F
A 0 B F  |  Z X C V  |  W X C V
```
Note that this is based on key position, and not on key label, so e.g. QWERTZ and DVORAK users wouldn't have to reach across their keyboards.
## Libraries used
[SDL2](http://libsdl.org/) by the libsdl organization\
[Dear ImGui](https://github.com/ocornut/imgui) by ocornut\
[tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) by vareille