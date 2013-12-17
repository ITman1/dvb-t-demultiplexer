/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2DefaultInputStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Hlavička obsahující výchozí třídu pro tvorbu MPEG2 vstupního streamu.
 *
 ******************************************************************************/

/**
 * @file MPEG2DefaultInputStream.h
 *
 * @brief Header which contains default class for MPEG2 input stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2DEFAULTINPUTSTREAM_H
#define MPEG2DEFAULTINPUTSTREAM_H

#include "MPEG2InputStream.h"
#include "../PSI/ServiceInformationTable.h"

using namespace std;

/**
 * Default class for MPEG2 input stream.
 */
class MPEG2DefaultInputStream: public MPEG2InputStream {
public:
    virtual void reset() = 0;
    virtual iterator &current() = 0;
    virtual iterator &end() = 0;
    virtual long currentFrameNo() = 0;
    virtual MPEG2InputStream &operator>>( MPEG2Packet &packet ) = 0;

    template <class Table>
    void readPSITable(shared_ptr<Table> &table, uint16_t pid, bool reset_flag = false) {
        if (reset_flag) {
            reset();
        }
        ServiceInformationTable::readTableFromStream(*this, table, pid);
    }

    template <class Table>
    void readPSITable(shared_ptr<Table> &table, bool reset_flag = false) {
        if (reset_flag) {
            reset();
        }
        ServiceInformationTable::readTableFromStream(*this, table);
    }
};

#endif // MPEG2DEFAULTINPUTSTREAM_H
