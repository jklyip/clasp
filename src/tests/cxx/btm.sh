#clang -cc1 -main-file-name testmpi.cc  -I /Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/release/include -I /Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/common/include -stdlib=libc++ -std=c++11  -fcxx-exceptions -fexceptions -o /tmp/testmpi.o -x c++ testmpi.cc

clang -cc1 -triple x86_64-apple-macosx10.9.0 -emit-obj -mrelax-all -disable-free -disable-llvm-verifier -main-file-name testmpi.cc -mrelocation-model pic -pic-level 2 -mdisable-fp-elim -masm-verbose -munwind-tables -target-cpu core2 -target-linker-version 224.1 -v -g -resource-dir /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/5.0 -I /Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/release/include -I /Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/common/include -stdlib=libc++ -std=c++11 -fdeprecated-macro -fdebug-compilation-dir /Users/meister/Development/cando/brcl/src/tests/cxx -ferror-limit 19 -fmessage-length 269 -stack-protector 1 -mstackrealign -fblocks -fobjc-runtime=macosx-10.9.0 -fobjc-dispatch-method=mixed  -fencode-extended-block-signature -fcxx-exceptions -fexceptions -fdiagnostics-show-option -fcolor-diagnostics -o ./testmpi.o -x c++ testmpi.cc


ld  -o testmpi -L/Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/release/lib -L/Users/meister/Development/cando/brcl/build/cando.app/Contents/Resources/externals/common/lib testmpi.o -lboost_mpi -lmpi_cxx -lmpi -lm -lc++ -lSystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/5.0/lib/darwin/libclang_rt.osx.a

cp testmpi $BRCL_APP_BIN_DIR