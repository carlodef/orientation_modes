#!/bin/bash

# copy the needed content
mkdir modes_1.0
cp -r src example README.txt makefile modes_1.0

# archive
zip -r modes_1.0.zip modes_1.0

# clean
rm -r modes_1.0
mv modes_1.0.zip ../
