TAG := c_darwin
REPOS_DIR := ~/repos
C := podman
RESTIC_REPO := ~/repos/c_darwin/.restic.db
clean:
	@podman rm -af||true
	@[[ "$C" -eq podman ]] && podman machine rm -f||true
	@brew uninstall podman docker||true
	@rm -rf ~/.local/share/containers

setup:
	@$(MAKE) -B -s machine >/dev/null 2>&1
	@command -v $C >/dev/null || brew install $C
	@brew services restart $C >/dev/null 2>&1
machine:
#	@mkdir -p ~/.local/share/containers/podman/qemu/cache
#	@find .xx2 -type f -name '*-qemu*qcow2*xz'|xargs echo mv % ~/.local/share/containers/podman/qemu/cache/.
	@[[ "$C" -eq podman ]] && $C machine init||true
	@[[ "$C" -eq podman ]] && $C machine start||true
	@[[ -d $(RESTIC_REPO) ]] || env RESTIC_PASSWORD=123123 restic init -r $(RESTIC_REPO)
#	@[[ -d $(RESTIC_REPO) ]] && env RESTIC_PASSWORD=123123 restic backup ~/.local/share/containers/podman/machine/qemu/cache/fedora-coreos-36.20221014.2.0-qemu.x86_64.qcow2.xz -r $(RESTIC_REPO)

build:
	@$C build -t $(TAG) -q .
R=c_greatest
R=c_vector
run:
	@clear
	@\
		echo \
		command env \
		$C run \
		-w /$(R) \
		--rm --name $(TAG) \
		--volume ~/repos/$(R):/$(R)\
		$(TAG):latest \
		bash -ex \<\<\< \"meson setup build --wipe \&\& meson compile -C build \&\& ls build \"
#		fish -il
#		echo sh -c "git reset --hard && git pull && rm -rf build && meson setup build"
docker: setup build run

restic-restore:
	env RESTIC_PASSWORD=123123 restic list snapshots -r /.restic.db
	env RESTIC_PASSWORD=123123 restic --target /Users/rick/c_deps/submodules -r /.restic.db
	env sh -c "cd /Users/rick/repos/c_deps && env CC=clang OBJC=clang CXX=clang++ OBJCXX=clang++ meson setup build"
