#!/bin/sh
export PATH=`pwd`/../host/bin:$PATH
make CROSS_COMPILE=mipsel-linux- && cp u-boot* ../tftp
