SGDK_VERSION=1.70
SGDK_FOLDER=SGDK
SED=sed
OS=$(shell uname -s)
ifeq ($(OS),Darwin)        # Mac OS X
    SED = gsed
endif
UID=$(shell id -u)
GID=$(shell id -g)

.PHONY: compile
compile:
	docker run -it --rm --user ${UID}:${GID} -v "${PWD}":/workdir -w /workdir  sgdk:${SGDK_VERSION}

compile-sgdk:
	make -f /sgdk/makefile.gen

pull:
	git submodule foreach git reset --hard
	git submodule update --init --remote --recursive
	cd ${SGDK_FOLDER} && git checkout tags/v${SGDK_VERSION}

patch:
	${SED} -i 's/FROM ubuntu/FROM amd64\/ubuntu/g' ${SGDK_FOLDER}/Dockerfile 

build-sgdk:	pull patch
	cd ${SGDK_FOLDER} && docker build . -t sgdk:${SGDK_VERSION}

shell:
	docker run -it --rm -v "${PWD}":/workdir -w /workdir --entrypoint=/bin/bash sgdk:${SGDK_VERSION} 

clean:
	rm -rf out/* build/* src/boot/*

format:
	clang-format -i src/main.c

.PHONY: build
build:	build-sgdk
	mkdir -p build && \
	cd build && \
	cmake SGDK_VERSION=${SGDK_VERSION} UID=${UID} GID=${GID} .. && \
	make