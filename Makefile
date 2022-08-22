default: all
##############################################################
include submodules/c_deps/etc/includes.mk
##############################################################
keybinds-yaml-to-json:
	@cat etc/keybinds.yaml|yaml2json |jq |tee etc/keybinds.json
local-setup: keybinds-yaml-to-json setup
