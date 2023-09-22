#!/bin/bash

OPGUI=$HOME/opgui
echo $OPGUI

echo [PROGRESS]  Backend Install INIT

cd $OPGUI/backend
echo $OPGUI/backend

mkdir build
cd build
cmake -DWITH_TESTS=OFF -DWITH_DOC=ON ..
make

cd $OPGUI/frontend/openAPI/generateApiClientFrontend
echo $OPGUI/frontend/openAPI/generateApiClientFrontend

rm -rf dist
rm -rf node_modules

yarn install && yarn build 
cd $OPGUI/frontend

rm -rf dist
rm -rf node_modules


yarn install && yarn build && cp -r dist ../backend/build 