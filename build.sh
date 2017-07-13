#!/bin/bash

CPU_CORES="$(nproc)"

build_exe () {
	local core_count=$1

	pushd build-unix
  	##cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
  	cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
  	##cmake -DCMAKE_BUILD_TYPE=Release ..
	make VERBOSE=1 -j$core_count
	popd
}

case "$1" in 

	clean) if [ ! -d build-unix ]; then
				echo "build directory does not exist. Terminating script.."
	   		else 
				rm -rvf build-unix
   		   fi
		;;

	rebuild) if [ -d build-unix ]; then
				rm -rvf build-unix
		     fi

			 mkdir -p -v build-unix

 			 ## call build function 
             build_exe ${CPU_CORES}
		;;

	*)	if [ ! -d build-unix ]; then
				echo "build directory does not exist. Creating directory.."
				mkdir -p -v build-unix
		fi
		
		 ## call build function 
         build_exe ${CPU_CORES}
		;;
esac

