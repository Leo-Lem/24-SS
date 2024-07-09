FROM mcr.microsoft.com/devcontainers/base:debian AS assemble

# Install required packages
RUN apt-get update -y &&\
    apt-get install -y --no-install-recommends ca-certificates perl wget perl-modules libfontconfig1 fontconfig sudo &&\
    rm -rf /var/lib/apt/lists/*

RUN adduser vscode sudo
USER vscode
CMD bin/bash

# Install texlive
ARG SCHEME
ARG MIRROR="https://mirror.ctan.org/systems/texlive/tlnet/"
WORKDIR /tmp
RUN wget "${MIRROR}/install-tl-unx.tar.gz" && \
    tar -xzf install-tl-unx.tar.gz
RUN sudo perl install-tl-*/install-tl --no-interaction --scheme=$SCHEME --location $MIRROR &&\
    sudo chown -R vscode:vscode /usr/local/texlive
RUN sudo rm -rf /tmp/*

# add texlive to PATH
ARG YEAR="2024"
ARG ARCH="aarch64-linux"
ENV PATH="${PATH}:/usr/local/texlive/${YEAR}/bin/${ARCH}"

# Install latex and latexmk
RUN tlmgr install latex latex-bin latexmk

# Split $TEX_PACKAGES string into separate strings using comma as delimiter, trim the resulting strings after split, and install them
ARG TEX_PACKAGES
RUN for package in $(echo $TEX_PACKAGES | tr ',' ' '); do \
        tlmgr install $package; \
    done