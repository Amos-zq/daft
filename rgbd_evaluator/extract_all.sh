#!/bin/bash

bin/extract_features /work/gossow/bags/frosties/*.bag -k 300 \
&& bin/extract_features /work/gossow/bags/granada/*.bag -k 300 \
&& bin/extract_features /work/gossow/bags/tum_poster/*.bag -k 500 \
&& bin/extract_features /work/gossow/bags/world_map/*.bag -k 400
