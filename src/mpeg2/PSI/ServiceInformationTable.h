/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ServiceInformationTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro přečtení servisní tabulky ze streamu
 *
 ******************************************************************************/

/**
 * @file ServiceInformationTable.h
 *
 * @brief Module for reading servise information table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef PSITABLE_H
#define PSITABLE_H

#include <memory>

#include "../streams/MPEG2InputStream.h"

using namespace std;

/**
 * Class representing general service information table
 */
class ServiceInformationTable
{
public:
    static shared_ptr<ServiceInformationTable> fromPacketStream(MPEG2InputStream &stream, uint16_t trackPID);

    template <class Table>
    static void readTableFromStream(MPEG2InputStream &stream, shared_ptr<Table> &table, uint16_t pid) {
        while (stream.current() != stream.end()) {
            table = Table::fromPacketStream(stream, pid);
            if (table) {
                return;
            }
        }

        table = shared_ptr<Table>(0);
    }

    template <class Table>
    static void readTableFromStream(MPEG2InputStream &stream, shared_ptr<Table> &table) {
        while (stream.current() != stream.end()) {
            table = Table::fromPacketStream(stream);
            if (table) {
                return;
            }
        }

        table = shared_ptr<Table>(0);
    }

    const unsigned int static PSI_MINSIZE           = 13;
    const unsigned int static PSI_HEADER_SIZE       = 3;
    const unsigned int static PSI_CRC_SIZE          = 4;

    uint16_t pid;
    uint8_t tableID;
    bool sectionSyntaxIndicator;
    uint16_t sectionLength;
    vector<uint8_t> section;
};

#endif // PSITABLE_H
