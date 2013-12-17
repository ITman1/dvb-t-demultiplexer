/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          PacketStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního streamu packetů.
 *
 ******************************************************************************/

/**
 * @file PacketStream.cpp
 *
 * @brief Module which implements class of the ouput stream of packets.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "MPEG2PacketStream.h"

/**
 * Counter of the total number of packets.
 */
long PacketStream::_processedPackets = 0;

/**
 * Puts new MPEG2 packet into the stream.
 * @return Reference to the current stream.
 */
PacketStream &PacketStream::put(const MPEG2Packet &) {
    _packetsInStream++;
    _processedPackets++;
    return *this;
}

/**
 * Constructs output packet stream.
 * @param PID PID of the packets which will be put into this stream.
 */
PacketStream::PacketStream(uint16_t PID) : _packetsInStream(0), PID(PID) {}

/**
 * Closes output stream
 */
void PacketStream::close() {}

/**
 * Returns number of the packets which were put into the stream.
 * @return Number of the packets which were put into the stream.
 */
long PacketStream::packetsInStream() {
    return _packetsInStream;
}

/**
 * Returns number of the packets which were processed by this class.
 * @return Number of the packets which were processed by this class.
 */
long PacketStream::processedPackets() {
    return _processedPackets;
}

/**
 * Returns PID of the stream.
 * @return PID of the stream.
 */
uint16_t PacketStream::getPID() {
    return PID;
}

/**
 * Calculates birate of the stream.
 *
 * @param bandwidth Bandwidth
 * @param codeRate Code rate
 * @param constellation Constellation
 * @param guardinterval Guard interval
 * @return Bitrate of the stream
 */
BitratePerPID PacketStream::calculateBitRate(const Bandwidth &bandwidth, const CodeRate &codeRate, const Constellation &constellation, const GuardInterval &guardinterval) {
    double maxBitRate = (423.0 / 544) * bandwidth.toValue() * codeRate.toValue();
    maxBitRate *= constellation.toValue() * guardinterval.toValue();
    BitratePerPID bitRatePerPID;
    bitRatePerPID.PID = PID;
    bitRatePerPID.bitrate = maxBitRate * ((double)_packetsInStream / _processedPackets) / 1000000;
    return bitRatePerPID;
}

PacketStream& PacketStream::operator<< (const MPEG2Packet& packet) {
    return put(packet);
}

