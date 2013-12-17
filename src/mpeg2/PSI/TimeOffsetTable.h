/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          TimeOffsetTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky posunu času
 *
 ******************************************************************************/

/**
 * @file TimeOffsetTable.h
 *
 * @brief Module for processing Time Offset Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef TIMEOFFSETTABLE_H
#define TIMEOFFSETTABLE_H

#include <memory>
#include <vector>

#include <ctime>

#include "Descriptors.h"
#include "ServiceInformationTable.h"

using namespace std;

/**
 * Class representing time offset table
 */
class TimeOffsetTable
{
protected:
    const uint16_t static TOT_HEADER_SIZE   = 5;
    const uint8_t static TOT_TABLE_ID       = 0x73;
public:
    TimeOffsetTable() {}
    TimeOffsetTable(ServiceInformationTable &table);

    static shared_ptr<TimeOffsetTable> fromPacketStream(MPEG2InputStream &stream);

    const uint16_t static TOT_PID           = 0x0014;

    struct tm timeUTC;
    Descriptors descriptors;
};

#endif // TIMEOFFSETTABLE_H
