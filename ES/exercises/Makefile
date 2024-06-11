# default parameters
# arch = arduino:avr:mega
arch = arduino:sam:arduino_due_x_dbg

# Check if the target is a known target
ifeq ($(filter $(MAKECMDGOALS),clean upload),)
  exercise := $(firstword $(MAKECMDGOALS))
endif

BUILD_DIR := .build
CHIPS := $(patsubst chips/%.chip.c, $(BUILD_DIR)/chips/%.chip.wasm, $(wildcard chips/*.chip.c))

.PHONY: all
all: $(CHIPS) $(BUILD_DIR)/exercise $(BUILD_DIR)/exercise/exercise.ino $(BUILD_DIR)/exercise/*.h compile

.PHONY: clean
clean:
		rm -rf $(BUILD_DIR)

# Compile the chip code to wasm
$(BUILD_DIR)/chips:
		mkdir -p $(BUILD_DIR)/chips

$(BUILD_DIR)/chips/%.chip.wasm: chips/%.chip.c | $(BUILD_DIR)/chips
	clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o $@ $< \
	&& cp chips/$*.chip.json $(BUILD_DIR)/chips/$*.chip.json

# Add libs
$(BUILD_DIR)/exercise/*.h: $(BUILD_DIR)/exercise lib/*.h
		cp lib/*.h $(BUILD_DIR)/exercise/

# Copy the exercise code to the build directory
$(BUILD_DIR)/exercise:
		mkdir -p $(BUILD_DIR)/exercise

$(BUILD_DIR)/exercise/exercise.ino: $(BUILD_DIR)/exercise src/$(exercise).ino
		cp src/$(exercise).ino $(BUILD_DIR)/exercise/exercise.ino

.PHONY: arduino-libs
arduino-libs:
	arduino-cli lib install $(shell cat lib/deps)

.PHONY: compile
compile: arduino-libs $(BUILD_DIR)/exercise/exercise.ino $(BUILD_DIR)/exercise/*.h
	arduino-cli compile -b $(arch) --build-path $(BUILD_DIR) $(BUILD_DIR)/exercise/exercise.ino

$(exercise):
	@$(MAKE) all exercise=$(exercise)

# arduino-cli upload -b arduino:sam:arduino_due_x_dbg --input-dir .build --port /dev/cu.usbmodem1101 .build/exercise/exercise.ino