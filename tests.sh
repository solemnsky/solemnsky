#! /nix/store/f2fi9x1fnwly702yqdlsjhy46pcgz5vi-bash-5.2-p15/bin/bash -e
export SOLEMNSKY_RESOURCES='/nix/store/yq1dm0nl5h9xaw4niwh8zdnjbvshyvym-solemnsky-media'
export PATH='/nix/store/188s80i0ggdpp2zdh5vqszjnqjnmqylm-p7zip-17.05/bin'
exec "/nix/store/y4mhzq819mc1zc52yk4pykfggf9zniyh-solemnsky-0.1/bin/solemnsky_tests_unwrapped"  "$@" 
