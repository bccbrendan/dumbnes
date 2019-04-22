#!/bin/bash
rm -f myeasylog.log
make && ./dumbnes_emu ~/code/nes-test-roms/nes_instr_test/rom_singles/01-implied.nes --verbose  &
exit 0
