/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketElementaryStreamFragment.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání PES fragmentů
 *
 ******************************************************************************/

/**
 * @file PacketElementaryStreamFragment.cpp
 *
 * @brief Module for processing the PES packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>
#include <iterator>

#include "PacketElementaryStreamFragment.h"

/**
 * Reads PES extension and construct object
 * @param data Vector with the PES extension.
 */
PacketElementaryStreamExtension::PacketElementaryStreamExtension(vector<uint8_t> &data) {
    if (data.size() < PES_EXTENSION_HEADER_SIZE) {
        throw runtime_error ("Unable to read extension of PES!");
    }
    byte1 = data[0];
    byte2 = data[1];
    length = data[2];

    if (data.size() < PES_EXTENSION_HEADER_SIZE + length) {
        throw runtime_error ("PES extension does not contain PES header data!");
    }

    totalLength = length + PES_EXTENSION_HEADER_SIZE;
}

/**
 * Reads PES header and construct object
 * @param data Vector with the PES header.
 */
PacketElementaryStreamHeader::PacketElementaryStreamHeader(vector<uint8_t> &data) {
    if (data.size() < PES_HEADER_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of PES!");
    }
    prefix = data[0] << 16;
    prefix = data[1] << 8;
    prefix = data[2];

    streamID = data[3];

    packetLength = data[4] << 8;
    packetLength = data[5];

    totalLength = PES_HEADER_HEADER_SIZE;
}

/**
 * Reads PES extension and construct object
 * @param data Vector with the PES extension.
 */
PacketElementaryStreamFragment::PacketElementaryStreamFragment(vector<uint8_t> &packet)
    : MPEG2Packet(packet) {
    initPESFragment();
}

/**
 * Gets PES fragment packet from the MPEG2 packet
 * @param packet MPEG2 packet
 * @return PES fragment packet
 */
PacketElementaryStreamFragment PacketElementaryStreamFragment::fromMPEG2Packet(const MPEG2Packet &packet) {
    PacketElementaryStreamFragment streamFragment;
    streamFragment.header = packet.header;
    streamFragment.adaptationField = packet.adaptationField;
    streamFragment.payload = packet.payload;

    streamFragment.initPESFragment();

    return streamFragment;
}

/**
 * Initializes PES fragment from MPEG2 background.
 */
void PacketElementaryStreamFragment::initPESFragment() {
    if (!payload) {
        return;
    }

    if (header->payloadUnitStartIndicator) {
        PESHeader = shared_ptr<PacketElementaryStreamHeader>(new PacketElementaryStreamHeader(payload->data));
        int dataOffset = PESHeader->totalLength;

        if (PESHeader->streamID == PacketElementaryStreamHeader::ID_PRIVATE_STREAM_1
            || (PESHeader->streamID >= PacketElementaryStreamHeader::ID_AUDIO_STREAM_START && PESHeader->streamID <= PacketElementaryStreamHeader::ID_AUDIO_STREAM_END)
            || (PESHeader->streamID >= PacketElementaryStreamHeader::ID_VIDEO_STREAM_START && PESHeader->streamID <= PacketElementaryStreamHeader::ID_VIDEO_STREAM_END)) {

            vector<uint8_t> extensionData(&payload->data[PESHeader->totalLength], &payload->data[payload->data.size()]);
            PESExtension = shared_ptr<PacketElementaryStreamExtension>(new PacketElementaryStreamExtension(extensionData));
            dataOffset += PESExtension->totalLength;
        }

        copy (&payload->data[dataOffset], &payload->data[payload->data.size()], back_inserter(streamData));
    } else {
        streamData = payload->data;
    }
}
