
commands:
	@grep _command_ *.c *.h|grep static|cut -d' ' -f3-5|cut -d'(' -f1|sort -u|grep '^_command_'|cut -d'_' -f3-100|sort -u
command_functions:
	@bash -exc 'printf "cmd_fxns: "; jo -a $(shell $(MAKE) -s commands)'  > vars/command_functions.yaml
	@cat vars/command_functions.yaml|yaml2json|jq -Mrc '.cmd_fxns'|jq -c && reset && echo command functions OK \
		&& \
	cat vars/command_functions.yaml|yaml2json|jq -Mrc '.cmd_fxns'|jq -c

all: commands command_functions

vars:
	@cat vars/*.yaml > .vars.yaml

gpp: commands command_functions vars
	@find . -name "*.*.gpp"|xargs -P 20 -I % env bash -ec "\
		gpp -x % -o /tmp/gpp-\`basename % .gpp\`\
		&& md5sum /tmp/gpp-\`basename % .gpp\` \`pwd\`/gpp-\`basename % .gpp\`\
		"|cut -d' ' -f 1,3
