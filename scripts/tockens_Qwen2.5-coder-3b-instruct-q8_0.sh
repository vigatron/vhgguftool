#!/bin/sh

MODEL_NAME="../model/Qwen2.5-coder-3b-instruct-q8_0.gguf"

../build/vhgguftool -model-file $MODEL_NAME -show-all > "$MODEL_NAME.txt"  #   2>&1
