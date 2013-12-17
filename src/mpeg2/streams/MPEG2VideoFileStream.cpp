/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2VideoFileStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního video streamu do soubor.
 *
 ******************************************************************************/

/**
 * @file MPEG2VideoFileStream.cpp
 *
 * @brief Module which defines class of the output video stream into the file.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <algorithm>

#include "MPEG2VideoFileStream.h"

/**
 * Callback method that is called by service base class - delivers PES packet.
 * Appending of the data into file is done here.
 * @param packetStream Video stream.
 */
void MPEG2VideoFileStream::onPacketRecieved(const PacketElementaryStream &) {
    //writeBuff(packetStream.streamData);
}

/**
 * Callback method that is called by service base class - delivers recieved fragment.
 * Appending of the data into file is done here.
 * @param streamFragment PES fragment.
 */
void MPEG2VideoFileStream::onFragmentRecieved(const PacketElementaryStreamFragment &streamFragment) {
    if (!sequenceHeaderFound) {
        auto it = search(streamFragment.streamData.begin(), streamFragment.streamData.end(), PacketElementaryStream::PES_DATA_HEADER_SEQUENCE.begin(), PacketElementaryStream::PES_DATA_HEADER_SEQUENCE.end());
        sequenceHeaderFound = it != streamFragment.streamData.end();

        if (sequenceHeaderFound) {
            vector<uint8_t> data(it, streamFragment.streamData.end());
            writeBuff(data);
        }
    } else {
        writeBuff(streamFragment.streamData);
    }
}

/**
 * Appends data into buffer. If buffer is full, then writes into the file.
 * @param data Data to be appended to the buffer.
 */
void MPEG2VideoFileStream::writeBuff(const vector<uint8_t> &data) {
    /* Buffer is full, flush the buffer. */
    if (_outputBufferPosition + data.size() > OUTPUT_BUFFER_SIZE) {
        flush();
    }

    /* Size of the buffer is not sufficient, save buffer to the file. */
    if (data.size() > OUTPUT_BUFFER_SIZE) {
        write(data, data.size());
    }
    /* SIze of buffer is sufficient, store data inside it. */
    else {
        copy(data.begin(), data.end(), &_outputBuffer[_outputBufferPosition]);
        _outputBufferPosition += data.size();
    }

}

/**
 * Writes data to the destination output file.
 * @param data Data to be added to the file.
 * @param size Size of the data.
 */
void MPEG2VideoFileStream::write(const vector<uint8_t> &data, int size) {
    output.write((char *)&data[0], size);
}

/**
 * Constructs new video stream.
 * @param PID PID of the video stream.
 */
MPEG2VideoFileStream::MPEG2VideoFileStream(uint16_t PID) : MPEG2ServiceStream(PID),
    _outputBuffer(vector<uint8_t>(OUTPUT_BUFFER_SIZE)), _outputBufferPosition(0), sequenceHeaderFound(false) {}

/**
 * Opends new video stream for writing.
 * @param filename Name of the file where to put the stream.
 */
void MPEG2VideoFileStream::open(string &filename) {
    output.open(filename, ios::out | ofstream::binary);
}

/**
 * Closes video file stream
 */
void MPEG2VideoFileStream::close() {
    flush();
    output.close();
}

/**
 * Tests if video stream is opened and correctly
 * @return True, if stream is good, otherwise false.
 */
bool MPEG2VideoFileStream::operator!(void) const {
    return output;
}

/**
 * Flushes buffer into the file.
 */
void MPEG2VideoFileStream::flush() {
    if (_outputBufferPosition > 0) {
        write(_outputBuffer, _outputBufferPosition);
    }
    _outputBufferPosition = 0;
}
