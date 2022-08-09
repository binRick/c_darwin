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
setup: keybinds-yaml-to-json

do-muon-setup: setup
	@muon setup build-muon
do-muon-clean:
	@rm -rf build-muon
do-muon-build:
	@muon samu -C build-muon
do-muon-install:
	@cd build-muon && muon install
do-muon-test:
	@cd build-muon && muon test
build-muon: do-muon-setup do-muon-build
muon: build-muon
build-meson: 

clean: do-muon-clean
	@rm -rf build

do-meson: do-meson-build
meson-build: build-meson
build-meson: do-meson-build do-build
do-meson-build:
	@eval cd . && {  meson build || { meson build --reconfigure || { meson build --wipe; } && meson build; }; }
do-install: all
	@meson install -C build
do-build:
	@meson compile -C build
do-test:
	@passh meson test -C build -v --print-errorlogs
install: do-install
test: do-test
build: do-meson do-build muon
uncrustify:
	@$(UNCRUSTIFY) -c submodules/c_deps/etc/uncrustify.cfg --replace $(TIDIED_FILES) 
uncrustify-clean:
	@find  . -type f -name "*unc-back*"|xargs -I % unlink %
fix-dbg:
	@$(SED) 's|, % s);|, %s);|g' -i $(TIDIED_FILES)
	@$(SED) 's|, % s);|, %c);|g' -i $(TIDIED_FILES)
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
meson-introspect-all:
	@meson introspect --all -i meson.build
meson-introspect-targets:
	@meson introspect --targets -i meson.build
meson-binaries:
	@meson introspect --targets  meson.build -i | jq 'map(select(.type == "executable").filename)|flatten|join("\n")' -Mrc|xargs -I % echo ./build/%
meson-binaries-loc:
	@make meson-binaries|xargs -I % echo %.c|sort -u|xargs Loc --files|bline -a bold:green -r yellow -R 1-6

do-pull-submodules-cmds:
	@command find submodules -type d -maxdepth 1|xargs -I % echo -e "sh -c 'cd % && git pull'"
run-binary:
	@clear; make meson-binaries | env FZF_DEFAULT_COMMAND= \
		fzf --reverse \
			--preview-window='follow,wrap,right,80%' \
			--bind 'ctrl-b:preview(make meson-build)' \
			--preview='env bash -c {} -v -a' \
			--ansi --border \
			--cycle \
			--header='Select Test Binary' \
			--height='100%' \
	| xargs -I % env bash -c "./%"
run-binary-nodemon:
	@make meson-binaries | fzf --reverse | xargs -I % nodemon -w build --delay 1000 -x passh "./%"
meson-tests-list:
	@meson test -C build --list
meson-tests:
	@{ make meson-tests-list; } |fzf \
		--reverse \
		--bind 'ctrl-b:preview(make meson-build)' \
		--bind 'ctrl-t:preview(make meson-tests-list)'\
		--bind 'ctrl-l:reload(make meson-tests-list)'\
		--bind 'ctrl-k:preview(make clean meson-build)'\
		--bind 'ctrl-y:preview-half-page-up'\
		--bind 'ctrl-u:preview-half-page-down'\
		--bind 'ctrl-/:change-preview-window(right,80%|down,90%,border-horizontal)' \
		--preview='\
			meson test --num-processes 1 -C build -v \
				--no-stdsplit --print-errorlogs {}' \
		--preview-window='follow,wrap,bottom,85%' \
        --ansi \
		--header='Select Test Case |ctrl+b:rebuild project|ctrl+k:clean build|ctrl+t:list tests|ctrl+l:reload test list|ctrl+/:toggle preview style|ctrl+y/u:scroll preview|'\
		--header-lines=0 \
        --height='100%'

keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
