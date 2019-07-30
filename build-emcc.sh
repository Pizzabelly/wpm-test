#!/bin/bash

if [ "$EMSDK" = "" ]
then
  echo "emsdk environment not set"
  exit 
fi

echo "compiling..."
emcc sentence_gen.c -O3 --preload-file words -o wpm.html \
  -s EXPORTED_FUNCTIONS="['_init', '_gen_sentence']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString"]'\
  -s MODULARIZE=1 -s 'EXPORT_NAME="wpm_gen"' -s TOTAL_MEMORY=67108864

echo "copying files.."
mv wpm.js website/
mv wpm.wasm website/
mv wpm.data website/
rm wpm.html
