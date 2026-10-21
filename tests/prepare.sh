#!/usr/bin/env bash
set -euo pipefail

project_dir=${1}
output_dir=${2}

mkdir -p "$output_dir"
for src in "$project_dir"/tests/data/test_*.S; do
  name="${src##*/}"
  ruby "$project_dir/as.rb" "$src" "$output_dir/${name%.S}.bin"
done
