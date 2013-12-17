/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2FileInputStream.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující vstupní stream ze souboru.
 *
 ******************************************************************************/

/**
 * @file MPEG2FileInputStream.h
 *
 * @brief Module which implements file input stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2FILEINPUTSTREAM_H
#define MPEG2FILEINPUTSTREAM_H

#include <fstream>
#include <memory>

#include "MPEG2FileInputIterator.h"
#include "MPEG2DefaultInputStream.h"

using namespace std;

/**
 * Class representing MPEG2 stream from the file.
 */
class MPEG2FileInputStream: public MPEG2DefaultInputStream, public ifstream {
public:
    MPEG2FileInputStream();

    virtual void reset() override;
    virtual iterator &current() override;
    virtual iterator &end() override;
    virtual long currentFrameNo() override;
    virtual MPEG2InputStream &operator>>( MPEG2Packet &packet ) override;

protected:
    shared_ptr<MPEG2FileInputIterator> currInputFileIter;
    MPEG2FileInputIterator endInputIter;
};

#endif // MPEG2FILEINPUTSTREAM_H
