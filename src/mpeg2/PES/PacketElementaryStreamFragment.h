/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketElementaryStreamFragment.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání PES fragmentů
 *
 ******************************************************************************/

/**
 * @file PacketElementaryStreamFragment.h
 *
 * @brief Module for processing the PES packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef PACKETELEMENTARYSTREAMFRAGMENT_H
#define PACKETELEMENTARYSTREAMFRAGMENT_H

#include <vector>

#include "../MPEG2Packet.h"

using namespace std;

/**
 * Class representing PES header
 */
class PacketElementaryStreamHeader {
    const unsigned int static PES_HEADER_HEADER_SIZE      = 6;
public:
    PacketElementaryStreamHeader() {}
    PacketElementaryStreamHeader(vector<uint8_t> &data);

    const unsigned int static ID_PRIVATE_STREAM_1      = 0xBD;
    const unsigned int static ID_PADDING_STREAM_1      = 0xBE;
    const unsigned int static ID_PRIVATE_STREAM_2      = 0xBF;
    const unsigned int static ID_AUDIO_STREAM_START    = 0xC0;
    const unsigned int static ID_AUDIO_STREAM_END      = 0xDF;
    const unsigned int static ID_VIDEO_STREAM_START    = 0xE0;
    const unsigned int static ID_VIDEO_STREAM_END      = 0xEF;

    uint32_t prefix;
    uint8_t streamID;
    uint16_t packetLength;
    uint16_t totalLength;
};

/**
 * Class representing PES Extension
 */
class PacketElementaryStreamExtension {
protected:
    const unsigned int static PES_EXTENSION_HEADER_SIZE      = 3;
public:
    PacketElementaryStreamExtension(vector<uint8_t> &data);

    uint8_t byte1; // TODO: finish processing data
    uint8_t byte2; // TODO: finish processing data
    uint8_t length;
    uint16_t totalLength;
};

/**
 * Class representing one PES fragment
 */
class PacketElementaryStreamFragment : public MPEG2Packet
{
protected:
    PacketElementaryStreamFragment() {}

    void initPESFragment();
public:
    PacketElementaryStreamFragment(vector<uint8_t> &packet);

    static PacketElementaryStreamFragment fromMPEG2Packet(const MPEG2Packet &packet);

    shared_ptr<PacketElementaryStreamHeader> PESHeader;
    shared_ptr<PacketElementaryStreamExtension> PESExtension;
    vector<uint8_t> streamData;
};

#endif // PACKETELEMENTARYSTREAMFRAGMENT_H
