SGDK_VERSION=1.65
SGDK_FOLDER=SGDK

SED=sed
ifeq ($(detected_OS),Darwin)        # Mac OS X
    SED = gsed
endif

.PHONY: compile
compile:
	docker run -it --rm -v "${PWD}":/workdir -w /workdir sgdk:${SGDK_VERSION}

compile-sgdk:
	make -f /sgdk/makefile.gen

pull:
	git submodule update --init --remote --recursive
	cd ${SGDK_FOLDER} && git checkout tags/v${SGDK_VERSION}

patch:
	${SED} -i 's/FROM ubuntu/FROM amd64\/ubuntu/g' ${SGDK_FOLDER}/Dockerfile 

build-sgdk:	pull patch
	cd ${SGDK_FOLDER} && docker build . -t sgdk:${SGDK_VERSION}

shell:
	docker run -it --rm -v "${PWD}":/workdir -w /workdir --entrypoint=/bin/bash sgdk:${SGDK_VERSION} 

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