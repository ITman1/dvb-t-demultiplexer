/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketElementaryStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání PES packetů
 *
 ******************************************************************************/

/**
 * @file PacketElementaryStream.cpp
 *
 * @brief Module for processing the PES packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include "PacketElementaryStream.h"

const static uint8_t PES_DATA_HEADER_PREFIX_ARR[] = { 0x00, 0x00, 0x01 };
const vector<uint8_t> PacketElementaryStream::PES_DATA_HEADER_PREFIX(PES_DATA_HEADER_PREFIX_ARR, PES_DATA_HEADER_PREFIX_ARR + sizeof PES_DATA_HEADER_PREFIX_ARR / sizeof PES_DATA_HEADER_PREFIX_ARR[ 0 ]);


const static uint8_t PES_DATA_HEADER_SEQUENCE_ARR[] = { 0x00, 0x00, 0x01, 0xB3 };
const vector<uint8_t> PacketElementaryStream::PES_DATA_HEADER_SEQUENCE(PES_DATA_HEADER_SEQUENCE_ARR, PES_DATA_HEADER_SEQUENCE_ARR + sizeof PES_DATA_HEADER_SEQUENCE_ARR / sizeof PES_DATA_HEADER_SEQUENCE_ARR[ 0 ]);

/**
 * Constructs PES packet from data vector
 * @param streamData Data vector which contains PES packet.
 */
PacketElementaryStream::PacketElementaryStream(vector<uint8_t> &streamData)
    :streamData(streamData) {

}

/**
 * Constructs PES packet from PES fragments
 * @param fragments vector of PES fragments.
 */
PacketElementaryStream::PacketElementaryStream(vector<PacketElementaryStreamFragment> &fragments) {
    if (fragments.empty()) {
        throw runtime_error ("There should be at least one fragment!");
    }

    PacketElementaryStreamFragment &firstFragment = fragments.front();
    if (!firstFragment.PESHeader) {
        throw runtime_error ("First fragment should contain PES header!");
    }

    PESHeader = firstFragment.PESHeader;
    PESExtension = firstFragment.PESExtension;

    /* Copies all fragments into current stream */
    for (const PacketElementaryStreamFragment &currFragment : fragments) {
        streamData.resize(streamData.size() + currFragment.streamData.size());
        copy ( currFragment.streamData.begin(), currFragment.streamData.end(), &streamData[streamData.size() - currFragment.streamData.size()] );
    }
}
