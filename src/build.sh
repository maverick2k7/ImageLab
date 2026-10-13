#!/usr/bin/env bash
# build.sh — Linux / MSYS2 bash build script for ImageLab
# Run from the project root: bash src/build.sh [target]
# Targets: imagelab (default), run_tests, make_images

CXX="g++"
FLAGS="-std=c++11 -O2 -Wall -Wextra -isystem src/third_party"

TARGET="${1:-imagelab}"

case "$TARGET" in

  imagelab)
    echo "Building imagelab..."
    $CXX $FLAGS \
        src/main.cpp \
        src/image_io.cpp \
        src/Image.cpp \
        src/ImageOps.cpp \
        src/BucketFill.cpp \
        src/HistoryManager.cpp \
        src/Pipeline.cpp \
        src/ImageLab.cpp \
        src/Inspector.cpp \
        src/Menu.cpp \
        -o imagelab
    echo "Done: imagelab"
    ;;

  run_tests)
    echo "Building run_tests..."
    $CXX $FLAGS \
        tests/test_main.cpp \
        tests/test_dynArray.cpp \
        tests/test_stack.cpp \
        tests/test_queue.cpp \
        tests/test_image_ops.cpp \
        tests/test_history.cpp \
        tests/test_pipeline.cpp \
        tests/test_fill.cpp \
        src/image_io.cpp \
        src/Image.cpp \
        src/ImageOps.cpp \
        src/BucketFill.cpp \
        src/HistoryManager.cpp \
        src/Pipeline.cpp \
        src/ImageLab.cpp \
        src/Inspector.cpp \
        src/Menu.cpp \
        -o run_tests
    echo "Done: run_tests"
    ;;

  make_images)
    echo "Building make_images..."
    $CXX $FLAGS \
        tests/make_test_images.cpp \
        src/image_io.cpp \
        src/Image.cpp \
        -o make_images
    echo "Done: make_images"
    ;;

  *)
    echo "Unknown target: $TARGET"
    echo "Usage: bash src/build.sh [imagelab|run_tests|make_images]"
    exit 1
    ;;

esac
