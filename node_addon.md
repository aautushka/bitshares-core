get the repo
```
git clone https://github.com/aautushka/bitshares-core.git
git submodule update --init --recursive
git submodule update --remote libraries/fc
```
   
configure a fpic build (do the Debug configuration, not sure is the Release is viable)
```
mkdir build && cd build
export PERL5LIB=$PWD/libraries/wallet/
cmake -DOPENSSL_INCLUDE_DIR=$OPENSSL10/include -DOPENSSL_SSL_LIBRARY    =$OPENSSL10/lib/libssl.a -DOPENSSL_CRYPTO_LIBRARY=$OPENSSL10/lib/libcrypto.a -DBOOST_ROOT=/opt/boost_1_63_0_fpic -DCMAKE_BUILD_TYPE=Debug ..
```

build fc with -fPIC: apply the patch to libraries/fc/CMakeLists.txt
```
 diff --git a/CMakeLists.txt b/CMakeLists.txt
 index 1f7787d..fd6f698 100644
 --- a/CMakeLists.txt
 +++ b/CMakeLists.txt
 @@ -80,7 +80,7 @@ else ( MSVC )
 ExternalProject_Add( project_secp256k1
 PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendor/secp256k1-zkp
 SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp
 - CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/vendor/secp256k1-zkp --with-bignum=no
 + CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/vendor/secp256k1-zkp --with-bignum=no CFLAGS=-fPIC CXXFLAGS=-fPIC --enable-shared --with-pic
 BUILD_COMMAND make
 INSTALL_COMMAND true
 BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/vendor/secp256k1-zkp/src/project_secp256k1-build/.libs/libsecp256k1.a
 @@ -95,6 +95,7 @@ else ( MSVC )
 ExternalProject_Get_Property(project_secp256k1 binary_dir)
 add_library(secp256k1 STATIC IMPORTED)
 + set_property(TARGET secp256k1 PROPERTY POSITION_INDEPENDENT_CODE ON)
 set_property(TARGET secp256k1 PROPERTY IMPORTED_LOCATION ${binary_dir}/.libs/libsecp256k1${CMAKE_STATIC_LIBRARY_SUFFIX})
 set_property(TARGET secp256k1 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/secp256k1-zkp/include)
 add_dependencies(secp256k1 project_secp256k1)
 @@ -249,6 +250,8 @@ list(APPEND sources ${fc_headers})
 add_subdirectory( vendor/websocketpp EXCLUDE_FROM_ALL )
 setup_library( fc SOURCES ${sources} LIBRARY_TYPE STATIC )
 +set_property(TARGET fc PROPERTY POSITION_INDEPENDENT_CODE ON)
 +
 install( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include )
 # begin readline stuff
```

apply the patch to libraries/libevem/libdevcore/CMakeLists.txt
```
diff --git a/libdevcore/CMakeLists.txt b/libdevcore/CMakeLists.txt
index 2e56635..023ddf9 100644
--- a/libdevcore/CMakeLists.txt
+++ b/libdevcore/CMakeLists.txt
@@ -17,6 +17,7 @@ SHA3.cpp
)

add_library(devcore ${sources} ${headers})
+set_property(TARGET devcore PROPERTY POSITION_INDEPENDENT_CODE ON)

add_dependencies(devcore BuildInfo.h)
```

build the universe
```
cd build && make
```

re-build secp256k1 with -fPIC (admittedly, a dirty hack, I know)
```
cd libraries/fc/vendor/secp256k1-zkp
./configure CFLAGS=-fPIC CXXFLAGS=-fPIC
make
```

build the addon
```
cd node_addon
node-gyp configure
node-gyp build
```

run the node.js console
```
node
```

import and use the b3 module
```
b3 = require('./b3.js')
b3.attach('ws://localhost:13010')
b3.get_global_properties()
```

If you see any problems with linking or loading the addon, perhaps, you need to start over again – the process is remarkably unreliable at this point. Clean the build files, clean secp256k-zkp:
```
rm -rf build/* && cd libraries/fc/vendor/secp256k1-zkp && make distclean
```

and rebuild the whole thing.

After making any changes to the code, you need to rebuild the addon
```
(cd build && make) && (cd node_addon && node-gyp rebuild)
```

