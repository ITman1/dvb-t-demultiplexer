/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2VideoFileStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního video streamu do soubor.
 *
 ******************************************************************************/

/**
 * @file MPEG2VideoFileStream.h
 *
 * @brief Module which defines class of the output video stream into the file.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2VIDEOFILESTREAM_H
#define MPEG2VIDEOFILESTREAM_H

#include <fstream>

#include "MPEG2ServiceStream.h"

/**
 * Class for outputtting video packets into file.
 */
class MPEG2VideoFileStream : public MPEG2ServiceStream {
protected:
    virtual void onPacketRecieved(const PacketElementaryStream &packetStream) override;
    virtual void onFragmentRecieved(const PacketElementaryStreamFragment &streamFragment) override;
    void writeBuff(const vector<uint8_t> &data);
    void write(const vector<uint8_t> &data, int size);

    vector<uint8_t> _outputBuffer;
    int _outputBufferPosition;
    static const unsigned int OUTPUT_BUFFER_SIZE = 1048576;
    ofstream output;
    bool sequenceHeaderFound;
public:
    MPEG2VideoFileStream(uint16_t PID);

    virtual void open(string &filename) override;
    virtual void close() override;
    virtual bool operator!(void) const override;
    void flush();
};

#endif // MPEG2VIDEOFILESTREAM_H
