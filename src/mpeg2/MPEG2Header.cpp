/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Header.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající hlavičku MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2Header.cpp
 *
 * @brief Module which reads header of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include "MPEG2Header.h"

using namespace std;

/**
 * Reads header of the MPEG2 packet.
 * @param field Data vector with the MPEG2 header.
 */
MPEG2Header::MPEG2Header(vector<uint8_t> &header)
{
    if (header.size() < 4) {
        throw runtime_error ("Expected packet with length at least 4 bytes");
    }

    uint8_t header_byte;
    uint8_t *packetPtr = &header[0];
    synByte = *packetPtr++;

    header_byte = *packetPtr++;

    transportErrorIndicator = header_byte & 0x80;
    payloadUnitStartIndicator = header_byte & 0x40;
    transportPriority = header_byte & 0x20;

    PID = (header_byte & 0x1F) << 8;
    PID |= *packetPtr++;

    header_byte = *packetPtr++;
    scramblingControl = static_cast<ScramblingControl>((header_byte & 0xC0) >> 6);
    adaptationFieldControl = static_cast<AdaptationFieldControl>((header_byte & 0x30) >> 4);
    continuityCounter = header_byte & 0x0F;
}
