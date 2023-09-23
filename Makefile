.PHONY: compile
compile:
	docker run -it --rm -v "${PWD}":/src sgdk:${SGDK_VERSION}

SGDK_VERSION=1.70
SGDK_FOLDER=SGDK
pull:
	git submodule update --init --remote --recursive
	cd ${SGDK_FOLDER} && git checkout tags/v${SGDK_VERSION}

build-sgdk:	pull
	cd ${SGDK_FOLDER} && docker build . -t sgdk:${SGDK_VERSION}

shell:
	docker run -it --rm -v "${PWD}":/src --entrypoint=/bin/bash sgdk:${SGDK_VERSION} 

clean:
	rm -rf out/* build/*

format:
	clang-format -i src/main.c

.PHONY: build
build:	build-sgdk
	mkdir -p build && \
	cd build && \
	cmake .. && \
	make