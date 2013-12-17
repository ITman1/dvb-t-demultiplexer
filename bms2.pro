#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T10:43:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = bms2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    src/bms2.cpp \
    src/mpeg2/MPEG2Payload.cpp \
    src/mpeg2/MPEG2Packet.cpp \
    src/mpeg2/MPEG2Header.cpp \
    src/mpeg2/MPEG2AdaptationField.cpp \
    src/mpeg2/PSI/ProgramAssociationTable.cpp \
    src/mpeg2/PSI/ProgramMapTable.cpp \
    src/mpeg2/PSI/NetworkInformationTable.cpp \
    src/mpeg2/PSI/ServiceInformationTable.cpp \
    src/mpeg2/PSI/ServiceDescriptionTable.cpp \
    src/mpeg2/PSI/EventInformationTable.cpp \
    src/mpeg2/PSI/Descriptors.cpp \
    src/mpeg2/PSI/TimeOffsetTable.cpp \
    src/mpeg2/PES/PacketElementaryStreamFragment.cpp \
    src/mpeg2/PES/PacketElementaryStream.cpp \
    src/mpeg2/MPEG2PacketStreams.cpp \
    src/mpeg2/MPEG2FileInputIterator.cpp \
    src/mpeg2/MPEG2FileInputStream.cpp \
    src/VideoFileStream.cpp \
    src/AudioFileStream.cpp \
    src/mpeg2/streams/MPEG2ServiceStream.cpp \
    src/mpeg2/streams/MPEG2PacketStream.cpp \
    src/mpeg2/streams/MPEG2FileInputStream.cpp \
    src/mpeg2/streams/MPEG2FileInputIterator.cpp \
    src/mpeg2/streams/MPEG2VideoFileStream.cpp \
    src/mpeg2/streams/MPEG2AudioFileStream.cpp

HEADERS += \
    src/mpeg2/MPEG2Payload.h \
    src/mpeg2/MPEG2Packet.h \
    src/mpeg2/MPEG2Header.h \
    src/mpeg2/MPEG2AdaptationField.h \
    src/mpeg2/PSI/ProgramAssociationTable.h \
    src/mpeg2/PSI/ProgramMapTable.h \
    src/mpeg2/PSI/NetworkInformationTable.h \
    src/mpeg2/PSI/ServiceInformationTable.h \
    src/mpeg2/PSI/ServiceDescriptionTable.h \
    src/mpeg2/PSI/EventInformationTable.h \
    src/mpeg2/PSI/Descriptors.h \
    src/mpeg2/PSI/TimeOffsetTable.h \
    src/mpeg2/PES/PacketElementaryStreamFragment.h \
    src/mpeg2/PES/PacketElementaryStream.h \
    src/mpeg2/MPEG2PacketStreams.h \
    src/mpeg2/MPEG2InputStream.h \
    src/miscellaneous.h \
    src/mpeg2/MPEG2DefaultInputStream.h \
    src/mpeg2/MPEG2InputIterator.h \
    src/mpeg2/MPEG2FileInputIterator.h \
    src/VideoFileStream.h \
    src/AudioFileStream.h \
    src/mpeg2/streams/MPEG2ServiceStream.h \
    src/mpeg2/streams/MPEG2PacketStream.h \
    src/mpeg2/streams/MPEG2InputStream.h \
    src/mpeg2/streams/MPEG2InputIterator.h \
    src/mpeg2/streams/MPEG2FileInputStream.h \
    src/mpeg2/streams/MPEG2FileInputIterator.h \
    src/mpeg2/streams/MPEG2DefaultInputStream.h \
    src/mpeg2/streams/MPEG2VideoFileStream.h \
    src/mpeg2/streams/MPEG2AudioFileStream.h
