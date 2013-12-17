/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2InputStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Hlavička obsahující základní třídu pro tvorbu MPEG2 vstupního streamu.
 *
 ******************************************************************************/

/**
 * @file MPEG2InputStream.h
 *
 * @brief Header which contains base class for MPEG2 input stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2INPUTSTREAM_H
#define MPEG2INPUTSTREAM_H

#include "MPEG2InputIterator.h"

using namespace std;

/**
 * Base class for all MPEG2 input streams.
 */
class MPEG2InputStream {
public:
    typedef MPEG2InputIterator iterator;

    virtual void reset() = 0;
    virtual iterator &current() = 0;
    virtual iterator &end() = 0;
    virtual long currentFrameNo() = 0;
    virtual MPEG2InputStream &operator>>( MPEG2Packet &packet ) = 0;
};

#endif // MPEG2INPUTSTREAM_H
