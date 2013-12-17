/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ProgramAssociationTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky programu
 *
 ******************************************************************************/

/**
 * @file ProgramAssociationTable.h
 *
 * @brief Module for processing Program Association Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef PROGRAMASSOCIATIONTABLE_H
#define PROGRAMASSOCIATIONTABLE_H

#include <vector>
#include <memory>

#include "ServiceInformationTable.h"

struct Program
{
public:
    const unsigned int static NIT_PROG_NUM   = 0x00;

    uint16_t programNum;
    uint16_t programPID;
};

class ProgramAssociationTable
{
protected:
    const unsigned int static PAT_HEADER_SIZE   = 5;
    const uint16_t static PAT_PID               = 0x0000;
    const uint8_t static PAT_TABLE_ID           = 0x00;

public:
    uint16_t transportStreamID;
    uint8_t versionNumber;
    bool currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    std::vector<Program> programs;

    ProgramAssociationTable() {}
    ProgramAssociationTable(ServiceInformationTable &table);

    static shared_ptr<ProgramAssociationTable> fromPacketStream(MPEG2InputStream &stream);

    bool containsProgramNum(uint16_t programNum);
};

#endif // PROGRAMASSOCIATIONTABLE_H
