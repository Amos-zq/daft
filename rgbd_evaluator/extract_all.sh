#!/bin/bash

gdb -ex run -ex q --args bin/extract_features /work/gossow/bags/frosties/*.bag -k 300 -r \
&& gdb -ex run -ex q --args bin/extract_features /work/gossow/bags/honeyloops/*.bag -k 300 -r \
&& gdb -ex run -ex q --args bin/extract_features /work/gossow/bags/granada/*.bag -k 300 -r \
&& gdb -ex run -ex q --args bin/extract_features /work/gossow/bags/tum_poster/*.bag -k 500 -r \
&& gdb -ex run -ex q --args bin/extract_features /work/gossow/bags/world_map/*.bag -k 400 -r
