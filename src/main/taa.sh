    "clang++" -E -x c++ -O0 -g -Wgnu-array-member-paren-init -Wno-deprecated-register -mmacosx-version-min=10.7 -fvisibility=default -std=c++11 -stdlib=libc++ -DCONFIG_VAR_COOL -DDEBUG_ASSERTS -DDEBUG_LOG_MPS_KINDS -DDEBUG_MPS -DDEBUG_MPS_AMS_POOL -DEXPAT -DINCLUDED_FROM_CLASP -DINHERITED_FROM_SRC -DREADLINE -DUSE_MPS -D_ADDRESS_MODEL_64 -D_DEBUG_BUILD -D_TARGET_OS_DARWIN -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -I"." -I"../../src" -I"../../src/asttooling/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/cffi/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/clbind/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/core/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/gctools/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/llvmo/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/mpip/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/serveEvent/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../../src/sockets/bin/clang-darwin-4.2.1/debug/link-static/mps-on" -I"../cffi" -I"../core" -I"../gctools" -I"../llvmo" -I"../serveEvent" -I"../sockets" -I"/Library/Frameworks/EPD64.framework/Versions/7.2/include/python2.7" -I"/Users/meister/Development/cando/clasp/build/cando.app/Contents/Resources/externals/common/include" -I"/Users/meister/Development/cando/clasp/build/cando.app/Contents/Resources/externals/release/include" -c -o "gc_interface.i" "gc_interface.cc"