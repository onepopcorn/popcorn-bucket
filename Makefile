include config.mk

EXAMPLES_DIR := ./examples
OUT_DIR := ./out

# Examples list. Exe filenames must be DOS friendly (8 characters long max)
MAP_basic-hello-dos := hello.exe
MAP_basic-gfx-mode-13h := gfx-13h.exe


# Default target (just for help)
help:
	@echo "Usage:"
	@echo "  make run <example>   - build and run an example in DOSBox"
	@echo "  make build <example> - only build an example"
	@echo "  make list            - show all available examples"

# List all possible examples
list:
	@echo "Available examples:"
	@$(foreach v,$(filter MAP_%,$(.VARIABLES)), \
		echo "  $(subst MAP_,,$(v))";)

# Compile and run the example on DOSBOX.
run:
	@KEY=$(filter-out $@,$(MAKECMDGOALS)); \
	$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/$$KEY FILENAME=$($(strip MAP_$(filter-out $@,$(MAKECMDGOALS)))); \
	if [ ! -f "$(OUT_DIR)/CWSDPMI.exe" ]; then \
		echo "CWSDPMI.exe not found. Copying from vendor..."; \
		cp vendor/CWSDPMI.exe $(OUT_DIR)/; \
	fi; \
	echo "RUNNING EXAMPLE $$KEY: $($(strip MAP_$(filter-out $@,$(MAKECMDGOALS))))"; \
	$(DOSBOX) "out\$($(strip MAP_$(filter-out $@,$(MAKECMDGOALS))))" &


# Just builds without running the example
build:
	@KEY=$(filter-out $@,$(MAKECMDGOALS)); \
	echo "BUILDING EXAMPLE: $$KEY"; \
	$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/$$KEY FILENAME=$($(strip MAP_$(filter-out $@,$(MAKECMDGOALS))))


clean:
	@KEY=$(filter-out $@,$(MAKECMDGOALS)); \
	echo "CLEANING EXAMPLE: $$KEY"; \
	$(MAKE) clean --no-print-directory -C $(EXAMPLES_DIR)/$$KEY FILENAME=$($(strip MAP_$(filter-out $@,$(MAKECMDGOALS))))


# Calls clean on each one of the examples
clean_all:
	@$(foreach v,$(filter MAP_%,$(.VARIABLES)), \
		KEY=$(subst MAP_,,$(v)); \
		echo "CLEANING $$KEY"; \
		$(MAKE) clean --no-print-directory -C $(EXAMPLES_DIR)/$$KEY FILENAME=$($v); \
	)

# Prevent make from thinking those are real targets
%:
	@:


.PHONY: help run clean clean_all build