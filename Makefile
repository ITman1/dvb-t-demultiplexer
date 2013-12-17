# File:      Makefile
# Course:    Bezdrátové a mobilní sítě
# Project:   Projekt č.2: Demultiplexing transportního streamu DVB-T 
# Author:    Radim Loskot, xlosko01@stud.fit.vutbr.cz
# Date:      Listopad 2013

# Usage:
#	- make               compile project - release version
#	- make pack          packs all required files to compile this project    
#	- make clean         clean temp compilers files    
#	- make debug         builds in debug mode    
#	- make release       builds in release mode 

# output project and package filename
SRC_DIR=src
OBJ_DIR=objs
TARGET=bms2
PACKAGE_NAME=xlosko01
PACKAGE_FILES=Makefile src

# C++ compiler, flags and libraries
INCLUDES = #-I c:/Development/BASS/c
CXX=g++
CXXFLAGS=$(CXXOPT) -Wall -pedantic -W -ansi -std=c++11 $(INCLUDES)
LIBS= -lbass #-L C:/Development/BASS/c -lbass

# Project files
OBJ_FILES=bms2.o \
          mpeg2/MPEG2Packet.o \
		  mpeg2/MPEG2Header.o \
		  mpeg2/MPEG2AdaptationField.o \
		  mpeg2/MPEG2Payload.o \
		  mpeg2/PSI/ServiceInformationTable.o \
		  mpeg2/PSI/ProgramAssociationTable.o \
		  mpeg2/PSI/NetworkInformationTable.o \
		  mpeg2/PSI/ProgramMapTable.o \
		  mpeg2/PSI/ServiceDescriptionTable.o \
		  mpeg2/PSI/EventInformationTable.o \
		  mpeg2/PSI/Descriptors.o \
		  mpeg2/PSI/TimeOffsetTable.o \
		  mpeg2/PES/PacketElementaryStream.o \
		  mpeg2/PES/PacketElementaryStreamFragment.o \
		  mpeg2/streams/MPEG2PacketStream.o \
		  mpeg2/streams/MPEG2FileInputIterator.o \
		  mpeg2/streams/MPEG2FileInputStream.o \
		  mpeg2/streams/MPEG2ServiceStream.o \
		  mpeg2/streams/MPEG2VideoFileStream.o \
		  mpeg2/streams/MPEG2AudioFileStream.o

SRC_FILES=bms2.cpp \
          mpeg2/MPEG2Packet.cpp \
		  mpeg2/MPEG2Header.cpp \
		  mpeg2/MPEG2AdaptationField.cpp \
		  mpeg2/MPEG2Payload.cpp \
		  mpeg2/PSI/ServiceInformationTable.cpp \
		  mpeg2/PSI/ProgramAssociationTable.cpp \
		  mpeg2/PSI/NetworkInformationTable.cpp \
		  mpeg2/PSI/ProgramMapTable.cpp \
		  mpeg2/PSI/ServiceDescriptionTable.o \
		  mpeg2/PSI/EventInformationTable.o \
		  mpeg2/PSI/Descriptors.cpp \
		  mpeg2/PSI/TimeOffsetTable.cpp \
		  mpeg2/PES/PacketElementaryStream.cpp \
		  mpeg2/PES/PacketElementaryStreamFragment.cpp \
		  mpeg2/streams/MPEG2PacketStream.cpp \
		  mpeg2/streams/MPEG2FileInputIterator.cpp \
		  mpeg2/streams/MPEG2FileInputStream.cpp \
		  mpeg2/streams/MPEG2ServiceStream.cpp \
		  mpeg2/streams/MPEG2VideoFileStream.cpp \
		  mpeg2/streams/MPEG2AudioFileStream.cpp

# Substitute the path
SRC=$(patsubst %,$(SRC_DIR)/%,$(SRC_FILES))
OBJ=$(patsubst %,$(OBJ_DIR)/%,$(OBJ_FILES))

# Universal rule for module compilation
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	g++ -c -o $@ $< $(CXXFLAGS)

# START RULE
all:
	make release

# Create compilation folders and compile the target
build: | $(OBJ_DIR) $(OBJ_DIR)/mpeg2 $(OBJ_DIR)/mpeg2/PES $(OBJ_DIR)/mpeg2/PSI $(OBJ_DIR)/mpeg2/streams $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/mpeg2:
	mkdir -p $(OBJ_DIR)/mpeg2

$(OBJ_DIR)/mpeg2/PES:
	mkdir -p $(OBJ_DIR)/mpeg2/PES

$(OBJ_DIR)/mpeg2/PSI:
	mkdir -p $(OBJ_DIR)/mpeg2/PSI

$(OBJ_DIR)/mpeg2/streams:
	mkdir -p $(OBJ_DIR)/mpeg2/streams

# Linking of modules into release program
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean pack run debug release

pack:
	zip -r $(PACKAGE_NAME).zip $(PACKAGE_FILES)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(TARGET)

debug:
	make -B build CXXOPT=-g3
	
release:
	make -B build CXXOPT=-O3
