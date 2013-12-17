/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního streamu packetů.
 *
 ******************************************************************************/

/**
 * @file PacketStream.h
 *
 * @brief Module which implements class of the ouput stream of packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2PACKETSTREAM_H
#define MPEG2PACKETSTREAM_H

#include "../PSI/Descriptors.h"
#include "../MPEG2Packet.h"

/**
 * Structore for storing bitrate of the stream
 */
struct BitratePerPID {
    inline bool operator< (const BitratePerPID& rhs) const {
        return bitrate < rhs.bitrate;
    }

    inline bool operator> (const BitratePerPID& rhs) const {
        return bitrate > rhs.bitrate;
    }

    uint16_t PID;
    double bitrate;
};

/**
 * Output stream for packets.
 */
class PacketStream {
protected:
    virtual PacketStream &put(const MPEG2Packet &);

    static long _processedPackets;
    mutable long _packetsInStream;
    uint16_t PID;
public:
    PacketStream(uint16_t PID);

    virtual void close();
    long packetsInStream();
    long processedPackets();
    uint16_t getPID();
    BitratePerPID calculateBitRate(const Bandwidth &bandwidth, const CodeRate &codeRate,
                                   const Constellation &constellation, const GuardInterval &guardinterval);

    PacketStream& operator<< (const MPEG2Packet& packet);
};
#endif // MPEG2PACKETSTREAM_H
