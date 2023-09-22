@echo off

SET OPGUI=%USERPROFILE%\opgui
echo %OPGUI%

:: Prerequisites -> node installation , path variables for msys
echo [PROGRESS]  Backend Install INIT

cd %OPGUI%
echo %OPGUI%
cd %OPGUI%\backend
echo %OPGUI%\backend

mkdir build
cd build
cmake -G "MinGW Makefiles" -DWITH_TESTS=OFF -DWITH_DOC=ON ..
cmake --build .

cd %OPGUI%\frontend\openAPI\generateApiClientFrontend
echo %OPGUI%\frontend\openAPI\generateApiClientFrontend

rm -rf dist
rm -rf node_modules

yarn install && yarn build 

cd %OPGUI%\frontend

rm -rf node_modules
rm -rf dist


yarn install && yarn build && cp -r dist ..\backend\build

