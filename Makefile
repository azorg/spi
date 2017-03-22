#----------------------------------------------------------------------------
OUT_NAME    := spi_test
OUT_DIR     := .
CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe" a.out
#----------------------------------------------------------------------------
# 1-st way to select source files
#SRCS := spi.c spi_test.c

#HDRS := spi.h

# 2-nd way to select source files
SRC_DIRS := .
HDR_DIRS := .
#----------------------------------------------------------------------------
DEFS    := -DSPI_DEBUG
#OPTIM  := -g -O0
OPTIM   := -Os
WARN    := -Wall -Wno-pointer-to-int-cast
CFLAGS  := $(WARN) $(OPTIM) $(DEFS) $(CFLAGS) -pipe
LDFLAGS := -lm -lrt -ldl -lpthread $(LDFLAGS)
PREFIX  := /opt
#----------------------------------------------------------------------------
#_AS  := @as
#_CC  := @gcc
#_CXX := @g++
#_LD  := @gcc

#_CC  := @clang
#_CXX := @clang++
#_LD  := @clang
#----------------------------------------------------------------------------
include Makefile.skel
#----------------------------------------------------------------------------

