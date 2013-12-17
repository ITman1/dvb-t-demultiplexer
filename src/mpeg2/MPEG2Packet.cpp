/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Packet.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající MPEG2 packet.
 *
 ******************************************************************************/

/**
 * @file MPEG2Packet.cpp
 *
 * @brief Module which reads MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include <cstdint>

#include "MPEG2Packet.h"

using namespace std;

/**
 * Reads MPEG2 packet from the data vector.
 * @param field Data vector with the MPEG2 packet.
 */
MPEG2Packet::MPEG2Packet()
    :header(0), adaptationField(0), payload(0)
{
}

/**
 * Reads MPEG2 packet from the data vector.
 * @param field Data vector with the MPEG2 packet.
 */
MPEG2Packet::MPEG2Packet(vector<uint8_t> &packet)
    :header(0), adaptationField(0), payload(0)
{
    if (packet.size() != PACKET_SIZE) {
        throw runtime_error ("Packet should have exactly the length 188 bytes!");
    }

    header = shared_ptr<MPEG2Header>(new MPEG2Header(packet));

    vector<uint8_t> adaptationAndPayload(&packet[4], &packet[packet.size()]);

    switch (header->adaptationFieldControl) {
    case AdaptationFieldControl::NoAdaptationFields:
        payload = shared_ptr<MPEG2Payload>(new MPEG2Payload(adaptationAndPayload));
        break;
    case AdaptationFieldControl::AdaptationFieldOnly:
        adaptationField = shared_ptr<MPEG2AdaptationField>(new MPEG2AdaptationField(adaptationAndPayload));
        break;
    case AdaptationFieldControl::AdaptationFieldAndPayload:
        adaptationField = shared_ptr<MPEG2AdaptationField>(new MPEG2AdaptationField(adaptationAndPayload));
        vector<uint8_t> payloadOnly(&adaptationAndPayload[adaptationField->totalLength], &adaptationAndPayload[adaptationAndPayload.size()]);
        payload = shared_ptr<MPEG2Payload>(new MPEG2Payload(payloadOnly));
        break;
    }
}
