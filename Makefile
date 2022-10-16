default: all
##############################################################
include submodules/c_deps/etc/includes.mk
MESON_DEFAULT_LIBRARY=shared
##############################################################
keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
local-setup: keybinds-yaml-to-json setup

test-dls:
	@meson test -C build --list|grep dls|xargs -I % echo -n '"%" '|xargs meson test -C build -v

test-mt:
	@meson test -v --list -C build|egrep '^mt>'|xargs -I % meson test -v -C build "%"
