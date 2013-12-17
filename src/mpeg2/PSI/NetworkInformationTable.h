/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          NetworkInformationTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání tabulky s událostmi
 *
 ******************************************************************************/

/**
 * @file NetworkInformationTable.h
 *
 * @brief Module for processing Network Information Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef NETWORKINFORMATIONTABLE_H
#define NETWORKINFORMATIONTABLE_H

#include <memory>
#include <vector>

#include "Descriptors.h"
#include "ServiceInformationTable.h"

using namespace std;

/**
 * Class representing transport stream of the network.
 */
class TransportStream {
protected:
    const unsigned int static STREAM_HEADER_SIZE        = 4;
public:
    TransportStream(vector<uint8_t> &data);

    uint16_t transportStreamID;
    uint16_t originalNetworkID;

    Descriptors descriptors;

    uint16_t totalLength;
};

/**
 * Class representing Network Information Table
 */
class NetworkInformationTable
{
protected:
    const uint16_t static NIT_HEADER_SIZE           = 5;

public:
    NetworkInformationTable() {}
    NetworkInformationTable(ServiceInformationTable &table);

    static shared_ptr<NetworkInformationTable> fromPacketStream(MPEG2InputStream &stream);
    static shared_ptr<NetworkInformationTable> fromPacketStream(MPEG2InputStream &stream, uint16_t pid);

    const uint16_t static NIT_DEFAULT_PID           = 0x0010;
    const uint8_t static NIT_ACTUAL_TABLE_ID        = 0x40;
    const uint8_t static NIT_DIFFERENT_TABLE_ID     = 0x41;

    uint16_t networkID;
    uint8_t versionNumber;
    bool currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    uint8_t tableID;

    Descriptors descriptors;
    vector<TransportStream> streams;
};

#endif // NETWORKINFORMATIONTABLE_H
