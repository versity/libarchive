FROM versity/python:2.7-devel
MAINTAINER Nic Henke <nic.henke@versity.com>

# Install libarchive RPMs to docker container to allow python/cython bits a build environment
COPY rpms/vsm-libarchive*.rpm /var/cache/

# epel for lz4. We might be able to remove that requirement, I don't think we use it by default
RUN yum install -y epel-release && yum makecache fast && \
    yum install -y /var/cache/vsm-libarchive*.rpm && \
    yum clean all && \
    rm -fr ~/.cache && rm -fr /var/cache/*
