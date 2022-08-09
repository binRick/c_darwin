default: all
##############################################################
include submodules/c_deps/etc/tools.mk
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
include submodules/c_deps/etc/includes.mk
all: build test
setup: keybinds-yaml-to-json

install: do-install
rm-make-logs:
	@rm .make-log* 2>/dev/null||true

dev-all: all

pull:
	@git pull


dev: nodemon
nodemon:
	@$(NODEMON) \
		--delay .3 \
		-w "*/*.c" -w "*/*.h" -w Makefle -w "*/meson.build" \
		-e c,h,sh,Makefile,build \
		-i build \
		-i build -i build-meson \
		-i submodules \
		-i .utils \
		-i .git \
			-x sh -- --norc --noprofile -c 'clear;make||true'
do-pull-submodules-cmds:
	@command find submodules -type d -maxdepth 1|xargs -I % echo -e "sh -c 'cd % && git pull'"
keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
