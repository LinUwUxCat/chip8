# 3DS-CHIP-8
## A CHIP-8 emulator/interpreter for the Nintendo 3DS
This is a CHIP-8 emulator/interpreter for the Nintendo 3DS, provided as a homebrew application.
## Roadmap
- Redo everything
## QnA
### What is this?
This is a separate branch of the main CHIP-8 project that aims to port my interpreter to the Nintendo 3DS (because why not?)\
However, lots have to be done in order to make it work for now. 
### Where is the sound?
Not implemented yet!
### Why won't [game] run / Why is [game] not working properly?
Only the CHIP-8 instructions are implemented, meaning [anything specific to other versions](https://games.gulrak.net/cadmium/chip8-opcode-table.html) may not work properly.\
If you think/know your game should run on CHIP-8, then try switching to the "CHIP-8" preset in settings.
### Despite the different modes, not all tests pass on [testing rom]
While I can't try all testing ROMs, I did try to pass what i could on the [test suite](https://github.com/Timendus/chip8-test-suite).\
However, there are a few intentional failures on that rom: 
- `Keypad test -> FX0A GETKEY` will say `NOT RELEASED` - See the next question. I am **not** planning to fix this.
- `Quirks -> CHIP-8` will (with the CHIP-8 preset) pass everything except `DISP.WAIT`. I am **not** planning to fix this.
- `Quirks -> XO-CHIP` will (with the XO-CHIP preset) pass everything except `CLIPPING`. I am planning to fix this.
### Why is the key input not how it was on CHIP-8 systems?
Because i find having to release keys for them to register is very confusing as a user, so i decided to send the input whenever the key is just pressed.\
This results in the [test suite](https://github.com/Timendus/chip8-test-suite) saying "not released" in the keypad test.\
Additionally, the input system (intentionally) only supports one input at once.

## Libraries used
TODO fill libraries