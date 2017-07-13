#!/bin/bash

#_architectures="x86_64-w64-mingw32 i686-w64-mingw32"
# Only build 64-bit Windows target for now
_architectures="x86_64-w64-mingw32"
CPU_CORES="$(nproc)"

build_exe () {
	local arch=$1
	local core_count=$2

	pushd build-$arch
	CMAKE_INCLUDE_PATH="/usr/"$arch"/include"
	echo "CMAKE_INCLUDE_PATH = "${CMAKE_INCLUDE_PATH}
	export CMAKE_INCLUDE_PATH
  	##$arch-cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
  	$arch-cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
  	##$arch-cmake -DCMAKE_BUILD_TYPE=Release ..
	make VERBOSE=1 -j$core_count
	popd
}

for _arch in ${_architectures}; do
	case "$1" in 

		clean) if [ ! -d build-${_arch} ]; then
					echo "build directory does not exist. Terminating script.."
		   		else 
					rm -rvf build-${_arch}
	   		   fi
			;;
	
		rebuild) if [ -d build-${_arch} ]; then
					rm -rvf build-${_arch}
			     fi

 				 mkdir -p -v build-${_arch}

	 			 ## call build function 
	             build_exe ${_arch} ${CPU_CORES}
			;;
	
		*)	if [ ! -d build-${_arch} ]; then
					echo "build directory does not exist. Creating directory.."
					mkdir -p -v build-${_arch}
			fi
			
 			 ## call build function 
             build_exe ${_arch} ${CPU_CORES}
			;;
	esac

done	

