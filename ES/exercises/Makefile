arch = arduino:avr:mega#arduino:avr:due
libs = TimerFour

ifndef ex
$(error ex(ercise number) is not set)
endif

.PHONY: all
all: clean chips exercise wokwi


.PHONY: clean
clean:
		rm -rf .build

.PHONY: chips
chips: .build/chips .build/chips/tb6612fng.chip.wasm .build/chips/tb6612fng.chip.json

.build/chips:
		mkdir -p .build/chips

.build/chips/tb6612fng.chip.wasm: .build/chips chips/tb6612fng.chip.c lib/wokwi-api.h
	clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o .build/chips/tb6612fng.chip.wasm chips/tb6612fng.chip.c

.build/chips/tb6612fng.chip.json: .build/chips chips/tb6612fng.chip.json
	  cp chips/tb6612fng.chip.json .build/chips

.PHONY: exercise
exercise: .build/exercise .build/exercise/exercise.ino .build/exercise/*.h

.build/exercise:
		mkdir -p .build/exercise

.build/exercise/exercise.ino: .build/exercise $(ex).ino
		cp $(ex).ino .build/exercise/exercise.ino

.build/exercise/*.h: .build/exercise lib/*.h
		cp lib/*.h .build/exercise/

.PHONY: arduino-libs
arduino-libs:
	arduino-cli lib install $(libs)

.PHONY: wokwi
wokwi: chips exercise arduino-libs
	arduino-cli compile -b $(arch) --build-path .build .build/exercise/exercise.ino