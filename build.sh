#!/bin/bash

src_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

work_dir="/tmp"
build_dir="$work_dir"/"sequencer_build"

mkdir -p $build_dir
cd $build_dir

cmake $src_dir

