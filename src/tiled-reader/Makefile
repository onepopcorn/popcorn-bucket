include ../../config.mk
include ../common.mk

DIST := ./dist

# also publish the exe + static assets (tmx, bmp...) into dist/
all: dist

dist: $(BIN)
	@mkdir -p $(DIST)
	@cp $(BIN) $(DIST)/
	@cp static/* $(DIST)/

clean: clean_dist
clean_dist:
	@rm -rf $(DIST)

.PHONY: dist clean_dist
