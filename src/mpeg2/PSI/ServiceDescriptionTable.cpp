/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ServiceDescriptionTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání servisní tabulky multiplexu
 *
 ******************************************************************************/

/**
 * @file ServiceDescriptionTable.cpp
 *
 * @brief Module for processing Service Description Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include "ServiceDescriptionTable.h"

/**
 * Constructs service from the data vector.
 * @param data Vector with the service.
 */
Service::Service(vector<uint8_t> &data) {
    if (data.size() < SERVICE_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of service, insufficient size of data!");
    }

    uint8_t *dataPtr = &data[0];

    /* Read service header */

    serviceID = *dataPtr++ << 8;
    serviceID |= *dataPtr++;

    EITScheduleFlag = *dataPtr & 0x02;
    EITPresentFollowingFlag = *dataPtr++ & 0x01;

    runningStatus = (RunningStatus)((*dataPtr++ & 0xE0) >> 5);
    freeCAMode = *dataPtr++ & 0x10;

    /* Read service descriptors */

    vector<uint8_t> descriptorsData(&data[SERVICE_HEADER_SIZE], &data[data.size()]);
    DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
    descriptors = descriptorLoop.descriptors;

    totalLength = SERVICE_HEADER_SIZE + descriptorLoop.totalLength;
}

/**
 * Constructs SDT from the general SIT.
 * @param table SIT table which contains SDT.
 */
ServiceDescriptionTable::ServiceDescriptionTable(ServiceInformationTable &table)
{
    if (table.section.size() < SDT_HEADER_SIZE + ServiceInformationTable::PSI_CRC_SIZE) {
        throw runtime_error ("Unable to read header of SDT, insufficient size of SIT section!");
    }

    if (table.pid != SDT_PID) {
        throw runtime_error ("Invalid PID for SDT! Expected PID: " + to_string(SDT_PID));
    }

    if (table.tableID != SDT_ACTUAL_TABLE_ID && table.tableID != SDT_DIFFERENT_TABLE_ID) {
        throw runtime_error ("Invalid table ID for SDT! Expected table ID: " + to_string(SDT_ACTUAL_TABLE_ID) + " or " + to_string(SDT_DIFFERENT_TABLE_ID));
    }

    uint8_t *dataPtr = &table.section[0];

    /* Read SDT header */

    transportStreamID = *dataPtr++ << 8;
    transportStreamID |= *dataPtr++;

    versionNumber = (*dataPtr & 0x3E) >> 1;
    currentNextIndicator = *dataPtr++ & 0x01;
    sectionNumber = *dataPtr++;
    lastSectionNumber = *dataPtr++;
    originalNetworkID = *dataPtr++ << 8;
    originalNetworkID |= *dataPtr++;

    /* Read SDT descriptors */

    uint16_t tableEnd = table.section.size() - ServiceInformationTable::PSI_CRC_SIZE;
    vector<uint8_t> serviceData(&table.section[SDT_HEADER_SIZE], &table.section[tableEnd]);
    while (!serviceData.empty()) {
        Service service(serviceData);
        services.push_back(service);
        serviceData.erase(serviceData.begin(), serviceData.begin() + service.totalLength);
    }
}

/**
 * Searches first SDT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @return Constructed SDT if located, otherwise null pointer
 */
shared_ptr<ServiceDescriptionTable> ServiceDescriptionTable::fromPacketStream(MPEG2InputStream &stream) {
    shared_ptr<ServiceInformationTable> sit = ServiceInformationTable::fromPacketStream(stream, ServiceDescriptionTable::SDT_PID);
    return (sit.get() != 0)? shared_ptr<ServiceDescriptionTable>(new ServiceDescriptionTable(*sit)) : 0;
}
