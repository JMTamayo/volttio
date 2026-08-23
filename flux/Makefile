.DEFAULT_GOAL := help

.PHONY: help build upload monitor flash clean format

help: ## Show available commands
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage: make \033[36m<command>\033[0m\n\nCommands:\n"} \
	/^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-14s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
	@echo ""

build: ## Compile the firmware
	pio run

upload: ## Flash the firmware to the device
	pio run --target upload

monitor: ## Open the serial monitor
	pio device monitor

flash: ## Flash the firmware and open the serial monitor
	pio run --target upload
	pio device monitor

clean: ## Remove build artifacts
	pio run --target clean

format: ## Format all C++ source files with clang-format
	@find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
