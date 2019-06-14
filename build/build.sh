#!/bin/sh

#================================================================
#   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
#   
#   @name          ：build.sh
#   @author        ：Afly
#   @date          ：2019.05.26
#   @description   ：
#
#================================================================

rm *.dat *.log -rf
make clean
#rm *.log
make -j32
#./fprint testAf multiply 7 9
