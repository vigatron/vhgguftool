#!/bin/sh

MODEL_NAME="../model/Qwen3.5-0.8B-bf16"

../build/vhgguftool -model-file $MODEL_NAME -show-all > "$MODEL_NAME.txt"  #   2>&1
