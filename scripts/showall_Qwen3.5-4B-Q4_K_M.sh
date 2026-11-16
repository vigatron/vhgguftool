#!/bin/sh

MODEL="Qwen3.5-4B-Q4_K_M"
MODEL_FILE="../models/$MODEL.gguf"
FNAME_OUT="../modelsdata/$MODEL.txt"

../build/vhgguftool -model-file $MODEL_FILE -show-all > "$FNAME_OUT"  #   2>&1
