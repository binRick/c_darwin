default: all
##############################################################
include submodules/c_deps/etc/includes.mk
##############################################################
keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
local-setup: keybinds-yaml-to-json setup

test-darwin-ls:
	@meson test -C build --list|grep darwin-ls|xargs -I % echo -n '"%" '|xargs meson test -C build -v
