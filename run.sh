#!/bin/bash

cd gcc
rm cdn.tar.gz
sh build.sh
#./bin/cdn cdn/data/second_test_case/0/case0.txt cdn/data/result.txt
./bin/cdn cdn/data/case0.txt cdn/data/result.txt