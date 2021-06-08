#!/usr/bin/env sh
cc -Iinclude -E -x c++ - -v < /dev/null 2>&1 \
    | awk '/End of search list./ { show=0 } \
           { if (show) printf "-I%s\n",$1 }; \
           /#include <...> search starts here:/ { show=1; }' \
    | tee compile_flags.txt
