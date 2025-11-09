PYTHON ?= python3
CMAKE ?= cmake
BUILD_DIR ?= build

.PHONY: all configure engine gui phase-map self-refine sample go-time clean theorist

all: configure
	$(CMAKE) --build $(BUILD_DIR)

configure:
	$(CMAKE) -S . -B $(BUILD_DIR)

engine: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_engine

gui: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_lab_gui

phase-map: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_phase_mapper

self-refine: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_self_refine

sample: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_sample

go-time: configure
	$(CMAKE) --build $(BUILD_DIR) --target trts_go_time

clean:
	rm -rf $(BUILD_DIR)

theorist:
	$(PYTHON) theorist_gpt.py --scan-all --suggest-tests --report
