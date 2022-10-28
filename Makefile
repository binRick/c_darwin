default: all
##############################################################
include submodules/c_deps/etc/includes.mk
MESON_DEFAULT_LIBRARY=shared
REPO_DIR=$(shell pwd)
WARN_LEVEL=0
##############################################################
keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
local-setup: keybinds-yaml-to-json setup

dls-tests:
	@meson test -C build --list|grep '^dls '|sort -u

dls-tests-capture:
	@$(MAKE) -s dls-tests | egrep '^dls Capture '

dls-test-cmds-capture:
	@$(MAKE) -s dls-tests-capture | xargs -I % echo meson test -v -C build \"%\"

dls-test-cmds:
	@$(MAKE) -s dls-tests | xargs -I % echo meson test -v -C build \"%\"

dls-test-select:
	@$(MAKE) -s dls-tests | fzf -m --ansi --height=50% --layout=reverse --border=sharp | xargs -I % echo meson test --no-rebuild -v -C build \"%\"|bash


test-dls:
	@meson test -C build --list|grep dls|xargs -I % echo -n '"%" '|xargs meson test -C build -v

test-mt:
	@meson test -v --list -C build|egrep '^mt>'|xargs -I % meson test -v -C build "%"
restic:
	@[[ -d $(REPO_DIR)/.restic.db ]] || env RESTIC_PASSWORD=123123 restic init -r $(REPO_DIR)/.restic.db
	@[[ -d ~/repos/c_darwin/.restic.db ]] && env RESTIC_PASSWORD=123123 restic backup ~/repos/c_deps/submodules -r ~/repos/c_darwin/.restic.db
	@(cd ~/repos/c_darwin/ && make docker -B)

gpp:
	@bash -exc '(find . -maxdepth 2 -type f -name "*.*.gpp" | xargs -P 16  -I % bash -exc "(cd \`dirname %\` && gpp -x \`basename %\` -o /tmp/gpp-\`basename % .gpp\`)")'
