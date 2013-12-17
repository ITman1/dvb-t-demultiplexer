/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2ServiceStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního streamu packetů služeb.
 *
 ******************************************************************************/

/**
 * @file MPEG2ServiceStream.cpp
 *
 * @brief Module which implements class of the ouput stream of service packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "MPEG2ServiceStream.h"

/**
 * Callback method which is called when new chunk of packets is available
 */
void MPEG2ServiceStream::onPacketRecieved(const PacketElementaryStream &) {}

/**
 * Callback method which is called when new fragment is available
 */
void MPEG2ServiceStream::onFragmentRecieved(const PacketElementaryStreamFragment &) {}

/**
 * Inserts new service packet into the stream.
 * @param packet Service packet.
 * @return Reference to the stream.
 */
PacketStream &MPEG2ServiceStream::put(const MPEG2Packet &packet) {
    PacketStream::put(packet);

    const PacketElementaryStreamFragment fragment = PacketElementaryStreamFragment::fromMPEG2Packet(packet);
    bool isStart = fragment.header->payloadUnitStartIndicator;
    started = (started)? started : isStart;

    /*if (fragment.header->PID == 0x101 && isStart) {
        int a = 5;
    } else if (fragment.header->PID == 0x101 && fragment.header->adaptationFieldControl == AdaptationFieldControl::AdaptationFieldOnly) {
        int b = 5;
        b = 1;
    }*/

    onFragmentRecieved(fragment);

    if (started) {
        if (isStart && !fragments.empty()) {
            PacketElementaryStream packetStream(fragments);
            fragments.clear();

            onPacketRecieved(packetStream);
        }

        fragments.push_back(fragment);
    }

    return *this;
}

/**
 * Constructs new service stream with the passed PID
 * @param PID PID of the service stream
 */
MPEG2ServiceStream::MPEG2ServiceStream(uint16_t PID)
    : PacketStream(PID), started(false) {

}
