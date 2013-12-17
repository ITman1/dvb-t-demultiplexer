/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Payload.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající payload MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2Payload.h
 *
 * @brief Module which reads payload of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2PAYLOAD_H
#define MPEG2PAYLOAD_H

#include <vector>

#include <cstdint>

/**
 * Class representing MPEG2 payload
 */
class MPEG2Payload
{
public:
    MPEG2Payload(std::vector<uint8_t> &payload);

    std::vector<uint8_t> data;
};

#endif // MPEG2PAYLOAD_H
