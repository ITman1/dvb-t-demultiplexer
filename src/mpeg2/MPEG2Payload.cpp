/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Payload.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající payload MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2Payload.cpp
 *
 * @brief Module which reads payload of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "MPEG2Payload.h"

using namespace std;

/**
 * Reads MPEG2 payload from the data vector.
 * @param field Data vector with the MPEG2 payload.
 */
MPEG2Payload::MPEG2Payload(vector<uint8_t> &payload)
{
    this->data = payload;
}
