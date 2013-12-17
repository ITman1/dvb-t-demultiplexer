/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          EventInformationTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky s událostmi
 *
 ******************************************************************************/

/**
 * @file EventInformationTable.h
 *
 * @brief Module for processing Event Information Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef EVENTINFORMATIONTABLE_H
#define EVENTINFORMATIONTABLE_H

#include <ctime>

#include "Descriptors.h"
#include "ServiceInformationTable.h"

using namespace std;

/**
 * Enum of statuses of the event.
 */
enum RunningStatus {
    Undefined           = 0x00,
    NotRunning          = 0x01,
    StartsInFewSeconds  = 0x02,
    Pausing             = 0x03,
    Running             = 0x04,
    ServiceOffAir       = 0x05,
    Reserved1           = 0x06,
    Reserved2           = 0x07
};

/**
 * Class representing an event.
 */
class Event {
protected:
    const unsigned int static EVENT_HEADER_SIZE        = 10;
public:

    Event(vector<uint8_t> &data);

    uint16_t eventID;

    struct tm startTime;
    struct tm duration;

    RunningStatus runningStatus;
    bool freeCAMode;
    Descriptors descriptors;

    uint16_t totalLength;
};

/**
 * Class representing Event Information table.
 */
class EventInformationTable
{
protected:
    const unsigned int static EIT_PID                   = 0x0012;
    const unsigned int static EIT_HEADER_SIZE           = 11;

public:
    EventInformationTable(ServiceInformationTable &table);
    EventInformationTable() {}

    static shared_ptr<EventInformationTable> fromPacketStream(MPEG2InputStream &stream);

    const unsigned int static EIT_PRESENT_TABLE_ID        = 0x4E;
    const unsigned int static EIT_SCHEDULE_STARTTABLE_ID  = 0x50;
    const unsigned int static EIT_SCHEDULE_ENDTABLE_ID    = 0x5F;

    uint8_t tableID;
    uint16_t serviceID;
    uint8_t versionNumber;
    bool currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    uint16_t transportStreamID;
    uint16_t originalNetworkID;
    uint8_t segmentLastSectionNumber;
    uint8_t lastTableID;

    vector<Event> events;
};

#endif // EVENTINFORMATIONTABLE_H
