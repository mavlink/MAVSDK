FROM dockcross/manylinux1-x64

ENV DEFAULT_DOCKCROSS_IMAGE manylinux1-x64-custom

ADD https://www.cpan.org/src/5.0/perl-5.28.1.tar.gz .
RUN tar xf perl-5.28.1.tar.gz && \
    cd perl-5.28.1 && \
    ./Configure -des > /dev/null && \
    make > /dev/null && \
    make install > /dev/null
