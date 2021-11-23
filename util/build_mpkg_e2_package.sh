#!/usr/bin/env bash

date=`date "+%d-%b-%Y"`

function clean {
	cd 2048-EZX/
	make -f Makefile.e2 clean
	cd -
}

function build_mpkg {
	cd 2048-EZX/
	make -f Makefile.e2 mpkg
	mv 2048-EZX_E2.mpkg "../2048_MotoEZX_E2_v1.0_$date.mpkg"
	cd -
}

function main {
	cd "`dirname $0`/../"
	build_mpkg
	clean
	cd -
	exit
}

main
