
# also change in archiso/liveusb/profiledef.sh
VERSION = 9

all: iso

# step 1: build toolchain

toolchain: toolchain/.stamp_built

toolchain/.stamp_built: toolchain/build.sh toolchain/Dockerfile
	./$<

# step 2a: build common bootstrap packages

bootstrap: bootstrap/.stamp_built

bootstrap/.stamp_built: bootstrap/build.sh toolchain
	./$<

# step 2b: build plugins bootstrap

plugins-bootstrap: plugins/.stamp_built

plugins/.stamp_built: plugins/build.sh plugins/Dockerfile toolchain
	./$<

# step 3a: build mod-os image

mod-os: mod-os/.stamp_built

mod-os/.stamp_built: mod-os/build.sh mod-os/Dockerfile bootstrap
	./$<

# step 3b: build plugins

plugins: plugins-bootstrap
	$(MAKE) -C plugins

# .PHONY: plugins

# step 4: build ISO

iso: archiso/output/mod-live-usb-v$(VERSION)-x86_64.iso

archiso/output/%.iso: archiso/build.sh archiso/Dockerfile archiso/liveusb/packages.x86_64 mod-os
	./$<

run:
	./archiso/run_archiso.sh -i ./archiso/output/mod-live-usb-v$(VERSION)-x86_64.iso

clean:
	make clean -C live-welcome
	rm -f toolchain/.stamp_built bootstrap/.stamp_built plugins/.stamp_built mod-os/.stamp_built
	rm -rf plugins/.stamps
	rm -rf plugins/bundles
	rm -rf toolchain/mod-workdir/*/build/*/.stamp_installed
	rm -rf toolchain/mod-workdir/*/build/*/.stamp_host_installed
	rm -rf toolchain/mod-workdir/*/build/*/.stamp_staging_installed
	rm -rf toolchain/mod-workdir/*/build/*/.stamp_target_installed
	rm -rf toolchain/mod-workdir/*/build/toolchain-external-undefined
	rm -rf toolchain/mod-workdir/*/build/toolchain-virtual
	rm -rf toolchain/mod-workdir/*/host
	rm -rf toolchain/mod-workdir/*/images
	rm -rf toolchain/mod-workdir/*/plugins
	rm -rf toolchain/mod-workdir/*/target
