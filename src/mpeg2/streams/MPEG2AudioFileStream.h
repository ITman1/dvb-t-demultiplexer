/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2AudioFileStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního audio streamu do soubor.
 *
 ******************************************************************************/

/**
 * @file MPEG2AudioFileStream.h
 *
 * @brief Module which defines class of the output audio stream into the file.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2AUDIOFILESTREAM_H
#define MPEG2AUDIOFILESTREAM_H

#include "MPEG2ServiceStream.h"

/**
 * Class for saving audio packets into file.
 */
class MPEG2AudioFileStream : public MPEG2ServiceStream {
protected:
    static bool bass_initialized;
    vector<uint8_t> data;
    string filename;
    bool audioHaderFound;

    static void writeWaw(string filename, vector<uint8_t> &data);

    virtual void onPacketRecieved(const PacketElementaryStream &packetStream) override;
    virtual void onFragmentRecieved(const PacketElementaryStreamFragment &streamFragment) override;
    void write(const vector<uint8_t> &streamData);
public:
    MPEG2AudioFileStream(uint16_t PID);

    virtual void open(string &filename) override;
    virtual void close() override;
    virtual bool operator!(void) const override;
};

#endif // MPEG2AUDIOFILESTREAM_H
