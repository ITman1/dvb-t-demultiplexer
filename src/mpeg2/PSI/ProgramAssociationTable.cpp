/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ProgramAssociationTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky programu
 *
 ******************************************************************************/

/**
 * @file ProgramAssociationTable.cpp
 *
 * @brief Module for processing Program Association Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <string>
#include <stdexcept>

#include "ProgramAssociationTable.h"

using namespace std;

/**
 * Constructs PAT from the general SIT.
 * @param table SIT table which contains PAT.
 */
ProgramAssociationTable::ProgramAssociationTable(ServiceInformationTable &table)
{
    if (table.section.size() < PAT_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of PAT, insufficient size of SIT section!");
    }

    if (table.pid != PAT_PID) {
        throw runtime_error ("Invalid PID for PAT! Expected PID: " + to_string(PAT_PID));
    }

    if (table.tableID != PAT_TABLE_ID) {
        throw runtime_error ("Invalid table ID for PAT! Expected table ID: " + to_string(PAT_TABLE_ID));
    }

    uint8_t *payloadPtr = &table.section[0];

    /* Read PAT header */

    transportStreamID = *payloadPtr++ << 8;
    transportStreamID |= *payloadPtr++;
    versionNumber = (*payloadPtr & 0x3E) >> 1;
    currentNextIndicator = *payloadPtr++ & 0x01;
    sectionNumber = *payloadPtr++;
    lastSectionNumber = *payloadPtr++;

    /* Read loop with program number to PID mapping */
    int programsSize = table.sectionLength - PAT_HEADER_SIZE - ServiceInformationTable::PSI_CRC_SIZE;
    for (int i = 0; i < programsSize / 4; i++) {
        Program program;
        program.programNum = *payloadPtr++ << 8;
        program.programNum |= *payloadPtr++;
        program.programPID = (*payloadPtr++ & 0x1F) << 8;
        program.programPID |= *payloadPtr++;
        programs.push_back(program);
    }
}

/**
 * Checks whether table contains program number
 * @param programNum Program table which will be searched.
 * @return True if program number exists in the table, otherwise false.
 */
bool ProgramAssociationTable::containsProgramNum(uint16_t programNum) {
    for (const Program &program : programs) {
        if (program.programNum == programNum) {
            return true;
        }

    }
    return false;
}

/**
 * Searches first PAT in the stream
 * @param stream Transport stream with the MPEG2 packets
 * @return Constructed PAT if located, otherwise null pointer
 */
shared_ptr<ProgramAssociationTable> ProgramAssociationTable::fromPacketStream(MPEG2InputStream &stream) {
    shared_ptr<ServiceInformationTable> sit = ServiceInformationTable::fromPacketStream(stream, ProgramAssociationTable::PAT_PID);
    return (sit.get() != 0)? shared_ptr<ProgramAssociationTable>(new ProgramAssociationTable(*sit)) : 0;
}
