clang++ --target=wasm32 -Os -nostdlib -Wl,--no-entry -Wl,--allow-undefined -Wl,--export,init -Wl,--export,step -Wl,--export,on_key_down src/snake.cpp -o dist/snake.wasm