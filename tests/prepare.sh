#!/usr/bin/env bash
set -euo pipefail

project_dir=${1}
output_dir=${2}
ext_dir=${3}

mkdir -p $output_dir
for src in $project_dir/tests/data/test_*.S; do
  name="${src##*/}"
  ruby -I $ext_dir $project_dir/as.rb $src $output_dir/${name%.S}.bin
done
