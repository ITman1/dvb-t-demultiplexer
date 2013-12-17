/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          NetworkInformationTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky s událostmi
 *
 ******************************************************************************/

/**
 * @file NetworkInformationTable.cpp
 *
 * @brief Module for processing Network Information Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "NetworkInformationTable.h"

#include <stdexcept>

using namespace std;

/**
 * Constructs transport stream from the data vector.
 * @param data Vector with the transport stream.
 */
TransportStream::TransportStream(vector<uint8_t> &data) {
    if (data.size() < STREAM_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of transport stream, insufficient size of data!");
    }

    uint8_t *dataPtr = &data[0];

    transportStreamID = *dataPtr++ << 8;
    transportStreamID |= *dataPtr++;

    originalNetworkID = *dataPtr++ << 8;
    originalNetworkID |= *dataPtr++;

    vector<uint8_t> descriptorsData(&data[STREAM_HEADER_SIZE], &data[data.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    descriptors = descriptorLoop.descriptors;

    totalLength = STREAM_HEADER_SIZE + descriptorLoop.totalLength;
}

/**
 * Constructs NIT from the general SIT.
 * @param table SIT table which contains NIT.
 */
NetworkInformationTable::NetworkInformationTable(ServiceInformationTable &table)
{
    if (table.section.size() < NIT_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of NIT, insufficient size of SIT section!");
    }

    if (table.tableID != NIT_ACTUAL_TABLE_ID && table.tableID != NIT_DIFFERENT_TABLE_ID) {
        throw runtime_error ("Invalid table ID for NIT! Expected table ID: " + to_string(NIT_ACTUAL_TABLE_ID) + " or " + to_string(NIT_DIFFERENT_TABLE_ID));
    }

    uint8_t *payloadPtr = &table.section[0];

    /* Read NIT header */

    networkID = *payloadPtr++ << 8;
    networkID |= *payloadPtr++;
    versionNumber = (*payloadPtr & 0x3E) >> 1;
    currentNextIndicator = *payloadPtr++ & 0x01;
    sectionNumber = *payloadPtr++;
    lastSectionNumber = *payloadPtr++;

    /* Read descriptor loop */

    vector<uint8_t> descriptorsData(&table.section[NIT_HEADER_SIZE], &table.section[table.section.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    descriptors = descriptorLoop.descriptors;

    /* Read length of the transport stream loop */

    payloadPtr = &table.section[NIT_HEADER_SIZE + descriptorLoop.totalLength];
    uint16_t streamsLength = (*payloadPtr++ & 0x0F) << 8;
    streamsLength |= *payloadPtr++;

    uint8_t readSize = descriptorLoop.totalLength + NIT_HEADER_SIZE + 2;
    if (table.section.size() < readSize + streamsLength) {
        throw runtime_error ("Unable to read descriptors of NIT, insufficient size of SIT section!");
    }

    /* Read transport stream loop */

    vector<uint8_t> streamsData(&table.section[readSize], &table.section[readSize + streamsLength]);
    while (!streamsData.empty()) {
        TransportStream stream(streamsData);
        streams.push_back(stream);
        streamsData.erase(streamsData.begin(), streamsData.begin() + stream.totalLength);
    }
}

/**
 * Searches first NIT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @return Constructed NIT if located, otherwise null pointer
 */
shared_ptr<NetworkInformationTable> NetworkInformationTable::fromPacketStream(MPEG2InputStream &stream) {
    return fromPacketStream(stream, NetworkInformationTable::NIT_DEFAULT_PID);
}

/**
 * Searches first NIT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @param pid PID of the packet with the NIT
 * @return Constructed NIT if located, otherwise null pointer
 */
shared_ptr<NetworkInformationTable> NetworkInformationTable::fromPacketStream(MPEG2InputStream &stream, uint16_t pid) {
    shared_ptr<ServiceInformationTable> sit = ServiceInformationTable::fromPacketStream(stream, pid);
    return (sit.get() != 0)? shared_ptr<NetworkInformationTable>(new NetworkInformationTable(*sit)) : 0;
}
