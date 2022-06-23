default: all
##############################################################
PASSH=$(shell command -v passh)
GIT=$(shell command -v git)
SED=$(shell command -v gsed||command -v sed)
NODEMON=$(shell command -v nodemon)
FZF=$(shell command -v fzf)
BLINE=$(shell command -v bline)
UNCRUSTIFY=$(shell command -v uncrustify)
PWD=$(shell command -v pwd)
FIND=$(shell command -v find)
EMBED_BINARY=$(shell command -v embed)
JQ_BIN=$(shell command -v jq)
##############################################################
DIR=$(shell $(PWD))
M1_DIR=$(DIR)
LOADER_DIR=$(DIR)/loader
EMBEDS_DIR=$(DIR)/embeds
VENDOR_DIR=$(DIR)/vendor
PROJECT_DIR=$(DIR)
MESON_DEPS_DIR=$(DIR)/meson/deps
VENDOR_DIR=$(DIR)/vendor
DEPS_DIR=$(DIR)/deps
BUILD_DIR=$(DIR)/build
ETC_DIR=$(DIR)/etc
MENU_DIR=$(DIR)/menu
DOCKER_DIR=$(DIR)/docker
LIST_DIR=$(DIR)/list
SOURCE_VENV_CMD=$(DIR)/scripts
VENV_DIR=$(DIR)/.venv
SCRIPTS_DIR=$(DIR)/scripts
ACTIVE_APP_DIR=$(DIR)/active-window
SOURCE_VENV_CMD = source $(VENV_DIR)/bin/activate
##############################################################
TIDIED_FILES = \
			   */*.h */*.c
##############################################################
all: build test
test-libs: 
	./build/window-utils-test/window-utils-test
	./build/app-utils-test/app-utils-test

clean:
	@rm -rf build
test: do-test
test-module:
	@echo TESTING MDDULE
	@./build/active-window-module-test/active-window-module-test -v | ./submodules/greatest/contrib/greenest
	@gtimeout .3 ./build/active-window-module-test/active-window-module-test --watch 2>/dev/null ||true
	@echo TESTING MDDULE OK; echo

test-bin:
	@echo TESTING
	@./build/active-window-test/active-window-test -v | ./submodules/greatest/contrib/greenest
	@gtimeout .3 ./build/active-window-test/active-window-test --watch 2>/dev/null ||true
	@echo TESTING OK; echo
do-meson: 
	@eval cd . && {  meson build || { meson build --reconfigure || { meson build --wipe; } && meson build; }; }
do-install: all
	@meson install -C build
do-build:
	@meson compile -C build
do-test:
	@passh meson test -C build -v --print-errorlogs
install: do-install
test: do-test
build: do-meson do-build
uncrustify:
	@$(UNCRUSTIFY) -c submodules/meson_deps/etc/uncrustify.cfg --replace $(TIDIED_FILES) 
uncrustify-clean:
	@find  . -type f -name "*unc-back*"|xargs -I % unlink %
fix-dbg:
	@$(SED) 's|, % s);|, %s);|g' -i $(TIDIED_FILES)
	@$(SED) 's|, % lu);|, %lu);|g' -i $(TIDIED_FILES)
	@$(SED) 's|, % d);|, %d);|g' -i $(TIDIED_FILES)
	@$(SED) 's|, % zu);|, %zu);|g' -i $(TIDIED_FILES)
	@$(SED) 's|, % llu);|, %llu);|g' -i $(TIDIED_FILES)
rm-make-logs:
	@rm .make-log* 2>/dev/null||true
tidy: rm-make-logs uncrustify uncrustify-clean fix-dbg

dev-all: all

pull:
	@git pull


dev: nodemon
nodemon:
	@$(PASSH) -L .nodemon.log $(NODEMON) -V -i build -w process -w process-test -w . -w '*/meson.build' --delay 1 -i '*/subprojects' -I  -w 'include/*.h' -w meson.build -w src -w Makefile -w loader/meson.build -w loader/src -w loader/include -i '*/embeds/*' -e tpl,build,sh,c,h,Makefile -x env -- bash -c 'reset;passh make||true'



meson-introspect-targets:
	@meson introspect --targets -i meson.build

meson-binaries:
	@meson introspect --targets  meson.build -i | jq 'map(select(.type == "executable").filename)|flatten|join("\n")' -Mrc
