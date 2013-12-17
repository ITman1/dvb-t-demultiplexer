/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          EventInformationTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky s událostmi
 *
 ******************************************************************************/

/**
 * @file EventInformationTable.cpp
 *
 * @brief Module for processing Event Information Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include "TimeOffsetTable.h"
#include "EventInformationTable.h"

/**
 * Constructs event from the data vector.
 * @param data Vector with the event.
 */
Event::Event(vector<uint8_t> &data) {
    if (data.size() < EVENT_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of transport stream, insufficient size of data!");
    }

    int readLength = 0;

    eventID = data[0] << 8;
    eventID |= data[1];
    readLength += 2;

    vector<uint8_t> dateTimeData(&data[readLength], &data[data.size()]);
    startTime = DateTime::parseDateTime(dateTimeData);
    readLength += 5;

    vector<uint8_t> timeData(&data[readLength], &data[data.size()]);
    duration = DateTime::parseTime(timeData);
    readLength += 2;

    runningStatus = (RunningStatus)((data[readLength] & 0xE0) >> 5);
    freeCAMode = data[readLength] & 0x10;
    readLength++;

    vector<uint8_t> descriptorsData(&data[EVENT_HEADER_SIZE], &data[data.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    descriptors = descriptorLoop.descriptors;

    totalLength = EVENT_HEADER_SIZE + descriptorLoop.totalLength;
}

/**
 * Constructs EIT from the general SIT.
 * @param table SIT table which contains EIT.
 */
EventInformationTable::EventInformationTable(ServiceInformationTable &table)
{
    if (table.section.size() < EIT_HEADER_SIZE + ServiceInformationTable::PSI_CRC_SIZE) {
        throw runtime_error ("Unable to read header of EIT, insufficient size of SIT section!");
    }

    tableID = table.tableID;

    uint8_t *dataPtr = &table.section[0];

    /* Read header of the EIT */

    serviceID = *dataPtr++ << 8;
    serviceID |= *dataPtr++;

    versionNumber = (*dataPtr & 0x3E) >> 1;
    currentNextIndicator = *dataPtr++ & 0x01;
    sectionNumber = *dataPtr++;
    lastSectionNumber = *dataPtr++;

    transportStreamID = *dataPtr++ << 8;
    transportStreamID |= *dataPtr++;

    originalNetworkID = *dataPtr++ << 8;
    originalNetworkID |= *dataPtr++;

    segmentLastSectionNumber = *dataPtr++;
    lastTableID = *dataPtr++;

    /* Read event loop */

    uint16_t tableEnd = table.section.size() - ServiceInformationTable::PSI_CRC_SIZE;
    vector<uint8_t> eventData(&table.section[EIT_HEADER_SIZE], &table.section[tableEnd]);
    while (!eventData.empty()) {
        Event event(eventData);
        events.push_back(event);
        eventData.erase(eventData.begin(), eventData.begin() + event.totalLength);
    }
}

/**
 * Searches first EIT from the stream
 * @param stream Transport stream with the MPEG2 packets
 * @return Constructed EIT if located, otherwise null pointer
 */
shared_ptr<EventInformationTable> EventInformationTable::fromPacketStream(MPEG2InputStream &stream) {
    shared_ptr<ServiceInformationTable> sit = ServiceInformationTable::fromPacketStream(stream, EventInformationTable::EIT_PID);
    return (sit.get() != 0)? shared_ptr<EventInformationTable>(new EventInformationTable(*sit)) : 0;
}
