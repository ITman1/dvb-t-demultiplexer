/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ProgramMapTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání programové tabulky
 *
 ******************************************************************************/

/**
 * @file ProgramMapTable.cpp
 *
 * @brief Module for processing Program Map Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "ProgramMapTable.h"

#include <stdexcept>

using namespace std;

/**
 * Constructs program stream from the data vector.
 * @param data Vector with the program stream.
 */
ProgramStream::ProgramStream(vector<uint8_t> &data) {
    if (data.size() < STREAM_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of program stream, insufficient size of data!");
    }

    uint8_t *dataPtr = &data[0];

    /* Read Program stream header */

    streamType = (StreamType)*dataPtr++;

    elementaryPID = (*dataPtr++ & 0x1F) << 8;
    elementaryPID |= *dataPtr++;

    /* Read descriptors */

    vector<uint8_t> descriptorsData(&data[STREAM_HEADER_SIZE], &data[data.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    ESDescriptors = descriptorLoop.descriptors;

    totalLength = STREAM_HEADER_SIZE + descriptorLoop.totalLength;
}

/**
 * Constructs PMT from the general SIT.
 * @param table SIT table which contains PMT.
 */
ProgramMapTable::ProgramMapTable(ServiceInformationTable &table)
{
    if (table.section.size() < PMT_HEADER_SIZE + ServiceInformationTable::PSI_CRC_SIZE) {
        throw runtime_error ("Unable to read header of SDT, insufficient size of SIT section!");
    }

    if (table.tableID != PMT_TABLE_ID) {
        throw runtime_error ("Invalid table ID for PMT! Expected table ID: " + to_string(PMT_TABLE_ID));
    }

    tableID = table.tableID;
    tablePID = table.pid;

    uint8_t *dataPtr = &table.section[0];

    /* Read PMT header */

    programNumber = *dataPtr++ << 8;
    programNumber |= *dataPtr++;

    versionNumber = (*dataPtr & 0x3E) >> 1;
    currentNextIndicator = *dataPtr++ & 0x01;
    sectionNumber = *dataPtr++;
    lastSectionNumber = *dataPtr++;

    PCR_PID = (*dataPtr++ & 0x1F) << 8;
    PCR_PID |= *dataPtr++;

    /* Read PMT descriptors */

    vector<uint8_t> descriptorsData(&table.section[PMT_HEADER_SIZE], &table.section[table.section.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    programDescriptors = descriptorLoop.descriptors;

    /* Read program stream loop */

    uint16_t tableStart = PMT_HEADER_SIZE + descriptorLoop.totalLength;
    uint16_t tableEnd = table.section.size() - ServiceInformationTable::PSI_CRC_SIZE;
    vector<uint8_t> streamData(&table.section[tableStart], &table.section[tableEnd]);
    while (!streamData.empty()) {
        ProgramStream stream(streamData);
        streams.push_back(stream);
        streamData.erase(streamData.begin(), streamData.begin() + stream.totalLength);
    }
}

/**
 * Searches first PMT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @param pid PID of the packet with the PMT
 * @return Constructed PMT if located, otherwise null pointer
 */
shared_ptr<ProgramMapTable> ProgramMapTable::fromPacketStream(MPEG2InputStream &stream, uint16_t pid) {
    shared_ptr<ServiceInformationTable> sit = ServiceInformationTable::fromPacketStream(stream, pid);
    return (sit.get() != 0)? shared_ptr<ProgramMapTable>(new ProgramMapTable(*sit)) : 0;
}
