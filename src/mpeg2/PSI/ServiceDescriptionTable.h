/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ServiceDescriptionTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání servisní tabulky multiplexu
 *
 ******************************************************************************/

/**
 * @file ServiceDescriptionTable.h
 *
 * @brief Module for processing Service Description Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef SERVICEDESCRIPTIONTABLE_H
#define SERVICEDESCRIPTIONTABLE_H

#include <memory>
#include <vector>

#include "Descriptors.h"
#include "ServiceInformationTable.h"
#include "EventInformationTable.h"

using namespace std;

/**
 * Class representing transport service.
 */
class Service {
protected:
    const unsigned int static SERVICE_HEADER_SIZE        = 3;
public:
    Service(vector<uint8_t> &data);

    uint16_t serviceID;
    bool EITScheduleFlag;
    bool EITPresentFollowingFlag;
    RunningStatus runningStatus;
    bool freeCAMode;

    Descriptors descriptors;

    uint16_t totalLength;
};

/**
 * Class representing service description table.
 */
class ServiceDescriptionTable
{
protected:
    const uint16_t static SDT_PID                    = 0x0011;
    const unsigned int static SDT_HEADER_SIZE        = 8;
public:
    ServiceDescriptionTable(ServiceInformationTable &table);
    ServiceDescriptionTable() {}

    static shared_ptr<ServiceDescriptionTable> fromPacketStream(MPEG2InputStream &stream);

    const uint8_t static SDT_ACTUAL_TABLE_ID         = 0x42;
    const uint8_t static SDT_DIFFERENT_TABLE_ID      = 0x46;

    uint16_t transportStreamID;
    uint8_t versionNumber;
    bool currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    uint16_t originalNetworkID;
    uint8_t tableID;

    vector<Service> services;
};

#endif // SERVICEDESCRIPTIONTABLE_H
