arch = arduino:avr:mega#arduino:avr:due
libs = TimerFour

.PHONY: all
all: .build/chips .build/chips/tb6612fng.chip.wasm .build/chips/dcmotor.chip.wasm .build/chips/*.chip.json .build/exercise .build/exercise/exercise.ino .build/exercise/*.h compile

.PHONY: clean
clean:
		rm -rf .build

.build/chips:
		mkdir -p .build/chips

.build/chips/tb6612fng.chip.wasm: .build/chips chips/tb6612fng.chip.c
	clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o .build/chips/tb6612fng.chip.wasm chips/tb6612fng.chip.c

.build/chips/dcmotor.chip.wasm: .build/chips chips/dcmotor.chip.c
	clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o .build/chips/dcmotor.chip.wasm chips/dcmotor.chip.c

.build/chips/*.chip.json: .build/chips chips/*.chip.json
	  cp chips/*.chip.json .build/chips

.build/exercise:
		mkdir -p .build/exercise

ifndef ex
$(error ex(ercise number) is not set)
endif

.build/exercise/exercise.ino: .build/exercise $(ex).ino
		cp $(ex).ino .build/exercise/exercise.ino

.build/exercise/*.h: .build/exercise lib/*.h
		cp lib/*.h .build/exercise/

.PHONY: arduino-libs
arduino-libs:
	arduino-cli lib install $(libs)

.PHONY: compile
compile: arduino-libs .build/exercise/exercise.ino .build/exercise/*.h
	arduino-cli compile -b $(arch) --build-path .build .build/exercise/exercise.ino

.PHONY: upload
upload: compile
	arduino-cli upload -b $(arch) --input-dir .build --port /dev/tty.usbmodem1101 .build/exercise/exercise.ino