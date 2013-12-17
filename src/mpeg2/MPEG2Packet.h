/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Packet.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající MPEG2 packet.
 *
 ******************************************************************************/

/**
 * @file MPEG2Packet.h
 *
 * @brief Module which reads MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2PACKET_H
#define MPEG2PACKET_H

#include <vector>
#include <memory>

#include "MPEG2Header.h"
#include "MPEG2AdaptationField.h"
#include "MPEG2Payload.h"

/**
 * Class representing MPEG2 packet
 */
class MPEG2Packet
{

public:
    MPEG2Packet();
    MPEG2Packet(std::vector<uint8_t> &packet);
    virtual ~MPEG2Packet() {}

    static const unsigned int PACKET_SIZE         = 188;
    static const unsigned int HEADER_SIZE         = 4;
    static const unsigned int PAYLOAD_MAXSIZE     = 184;

    std::shared_ptr<MPEG2Header> header;
    std::shared_ptr<MPEG2AdaptationField> adaptationField;
    std::shared_ptr<MPEG2Payload> payload;
};

#endif // MPEG2PACKET_H
