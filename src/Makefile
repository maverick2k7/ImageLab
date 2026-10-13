# Makefile for ImageLab — run from inside src/ OR from the project root.
# Uses explicit file lists (no wildcards) as required by the assignment.

CXX      = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -isystem src/third_party

# Detect whether we are being called from the project root or from src/.
# We always want paths relative to the project root.
ROOT     = ..

# --------------------------------------------------------------------------
# Source file lists (explicit — no wildcards)
# --------------------------------------------------------------------------
MAIN_SRC = \
    $(ROOT)/src/main.cpp \
    $(ROOT)/src/image_io.cpp \
    $(ROOT)/src/Image.cpp \
    $(ROOT)/src/ImageOps.cpp \
    $(ROOT)/src/BucketFill.cpp \
    $(ROOT)/src/HistoryManager.cpp \
    $(ROOT)/src/Pipeline.cpp \
    $(ROOT)/src/ImageLab.cpp \
    $(ROOT)/src/Inspector.cpp \
    $(ROOT)/src/Menu.cpp

# All src .cpp files EXCEPT main.cpp (shared by test and make_images targets)
LIB_SRC = \
    $(ROOT)/src/image_io.cpp \
    $(ROOT)/src/Image.cpp \
    $(ROOT)/src/ImageOps.cpp \
    $(ROOT)/src/BucketFill.cpp \
    $(ROOT)/src/HistoryManager.cpp \
    $(ROOT)/src/Pipeline.cpp \
    $(ROOT)/src/ImageLab.cpp \
    $(ROOT)/src/Inspector.cpp \
    $(ROOT)/src/Menu.cpp

TEST_SRC = \
    $(ROOT)/tests/test_main.cpp \
    $(ROOT)/tests/test_dynArray.cpp \
    $(ROOT)/tests/test_stack.cpp \
    $(ROOT)/tests/test_queue.cpp \
    $(ROOT)/tests/test_image_ops.cpp \
    $(ROOT)/tests/test_history.cpp \
    $(ROOT)/tests/test_pipeline.cpp \
    $(ROOT)/tests/test_fill.cpp

MKIMG_SRC = \
    $(ROOT)/tests/make_test_images.cpp \
    $(ROOT)/src/image_io.cpp \
    $(ROOT)/src/Image.cpp

# --------------------------------------------------------------------------
# Output executable names (placed in project root so images/ path works)
# --------------------------------------------------------------------------
IMAGELAB   = $(ROOT)/imagelab
RUN_TESTS  = $(ROOT)/run_tests
MAKE_IMGS  = $(ROOT)/make_images

# --------------------------------------------------------------------------
# Targets
# --------------------------------------------------------------------------
.PHONY: all imagelab run_tests make_images clean

all: imagelab

imagelab:
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(IMAGELAB)

run_tests:
	$(CXX) $(CXXFLAGS) $(TEST_SRC) $(LIB_SRC) -o $(RUN_TESTS)

make_images:
	$(CXX) $(CXXFLAGS) $(MKIMG_SRC) -o $(MAKE_IMGS)

clean:
	rm -f $(IMAGELAB) $(RUN_TESTS) $(MAKE_IMGS) \
	       $(IMAGELAB).exe $(RUN_TESTS).exe $(MAKE_IMGS).exe \
	       $(ROOT)/*.o $(ROOT)/src/*.o $(ROOT)/tests/*.o
