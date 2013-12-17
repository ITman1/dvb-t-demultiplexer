/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketElementaryStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání PES packetů
 *
 ******************************************************************************/

/**
 * @file PacketElementaryStream.h
 *
 * @brief Module for processing the PES packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef PACKETELEMENTARYSTREAM_H
#define PACKETELEMENTARYSTREAM_H

#include "PacketElementaryStreamFragment.h"

using namespace std;

/**
 * Class for storing packet element stream fragments.
 */
class PacketElementaryStream
{
protected:
    PacketElementaryStream() {}

public:
    PacketElementaryStream(vector<uint8_t> &streamData);
    PacketElementaryStream(vector<PacketElementaryStreamFragment> &fragments);

    const unsigned int static PES_DATA_HEADER_SIZE = 4;
    const static vector<uint8_t> PES_DATA_HEADER_PREFIX;
    const static vector<uint8_t> PES_DATA_HEADER_SEQUENCE;

    shared_ptr<PacketElementaryStreamHeader> PESHeader;
    shared_ptr<PacketElementaryStreamExtension> PESExtension;
    vector<uint8_t> streamData;
};

#endif // PACKETELEMENTARYSTREAM_H
