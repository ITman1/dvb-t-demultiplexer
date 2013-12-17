/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          TimeOffsetTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky posunu času
 *
 ******************************************************************************/

/**
 * @file TimeOffsetTable.cpp
 *
 * @brief Module for processing Time Offset Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "TimeOffsetTable.h"

#include <stdexcept>

using namespace std;

/**
 * Constructs TOT from the general SIT.
 * @param table SIT table which contains TOT.
 */
TimeOffsetTable::TimeOffsetTable(ServiceInformationTable &table)
{
    if (table.section.size() < TOT_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of TOT, insufficient size of SIT section!");
    }

    if (table.pid != TOT_PID) {
        throw runtime_error ("Invalid PID for TOT! Expected PID: " + to_string(TOT_PID));
    }

    if (table.tableID != TOT_TABLE_ID) {
        throw runtime_error ("Invalid table ID for TOT! Expected table ID: " + to_string(TOT_TABLE_ID));
    }

    /* Read TOT header */

    timeUTC = DateTime::parseDateTime(table.section);

    /* Read TOT descriptors */

    if (table.section.size() > TOT_HEADER_SIZE) {
        vector<uint8_t> descriptorsData(&table.section[TOT_HEADER_SIZE], &table.section[table.section.size()]);
        DescriptorLoop descriptorLoop = DescriptorFactory::readDescriptorLoop(descriptorsData);
        descriptors = descriptorLoop.descriptors;
    }
}

/**
 * Searches first TOT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @return Constructed TOT if located, otherwise null pointer
 */
shared_ptr<TimeOffsetTable> TimeOffsetTable::fromPacketStream(MPEG2InputStream &stream) {
    shared_ptr<ServiceInformationTable> sit;
    while (!sit) {
        sit = ServiceInformationTable::fromPacketStream(stream, TimeOffsetTable::TOT_PID);
        if (sit->tableID != TOT_TABLE_ID) {
            sit.reset();
        }
    }
    return (sit)? shared_ptr<TimeOffsetTable>(new TimeOffsetTable(*sit)) : 0;
}
