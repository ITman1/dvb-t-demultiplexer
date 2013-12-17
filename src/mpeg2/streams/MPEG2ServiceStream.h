/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2ServiceStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního streamu packetů služeb.
 *
 ******************************************************************************/

/**
 * @file MPEG2ServiceStream.h
 *
 * @brief Module which implements class of the ouput stream of service packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2SERVICESTREAM_H
#define MPEG2SERVICESTREAM_H

#include "MPEG2PacketStream.h"
#include "../PES/PacketElementaryStream.h"

/**
 * Class for streaming packets into output stream.
 */
class MPEG2ServiceStream : public PacketStream {
protected:
    vector<PacketElementaryStreamFragment> fragments;
    bool started;

    virtual void onPacketRecieved(const PacketElementaryStream &);
    virtual void onFragmentRecieved(const PacketElementaryStreamFragment &streamFragment);
    virtual PacketStream &put(const MPEG2Packet &packet);
public:
    MPEG2ServiceStream(uint16_t PID);

    virtual void open(string &filename) = 0;
    virtual bool operator!(void) const = 0;
};

#endif // MPEG2SERVICESTREAM_H
