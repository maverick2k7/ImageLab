@echo off
REM build.bat — Windows cmd build script for ImageLab
REM Run from the project root: src\build.bat [target]
REM Targets: imagelab (default), run_tests, make_images

set CXX=g++
set FLAGS=-std=c++11 -O2 -Wall -Wextra -isystem src/third_party

if "%1"=="" goto imagelab
if "%1"=="imagelab" goto imagelab
if "%1"=="run_tests" goto run_tests
if "%1"=="make_images" goto make_images

echo Unknown target: %1
echo Usage: build.bat [imagelab^|run_tests^|make_images]
exit /b 1

:imagelab
echo Building imagelab...
%CXX% %FLAGS% ^
    src/main.cpp ^
    src/image_io.cpp ^
    src/Image.cpp ^
    src/ImageOps.cpp ^
    src/BucketFill.cpp ^
    src/HistoryManager.cpp ^
    src/Pipeline.cpp ^
    src/ImageLab.cpp ^
    src/Inspector.cpp ^
    src/Menu.cpp ^
    -o imagelab.exe
if %ERRORLEVEL%==0 (echo Done: imagelab.exe) else (echo FAILED & exit /b 1)
goto end

:run_tests
echo Building run_tests...
%CXX% %FLAGS% ^
    tests/test_main.cpp ^
    tests/test_dynArray.cpp ^
    tests/test_stack.cpp ^
    tests/test_queue.cpp ^
    tests/test_image_ops.cpp ^
    tests/test_history.cpp ^
    tests/test_pipeline.cpp ^
    tests/test_fill.cpp ^
    src/image_io.cpp ^
    src/Image.cpp ^
    src/ImageOps.cpp ^
    src/BucketFill.cpp ^
    src/HistoryManager.cpp ^
    src/Pipeline.cpp ^
    src/ImageLab.cpp ^
    src/Inspector.cpp ^
    src/Menu.cpp ^
    -o run_tests.exe
if %ERRORLEVEL%==0 (echo Done: run_tests.exe) else (echo FAILED & exit /b 1)
goto end

:make_images
echo Building make_images...
%CXX% %FLAGS% ^
    tests/make_test_images.cpp ^
    src/image_io.cpp ^
    src/Image.cpp ^
    -o make_images.exe
if %ERRORLEVEL%==0 (echo Done: make_images.exe) else (echo FAILED & exit /b 1)
goto end

:end
