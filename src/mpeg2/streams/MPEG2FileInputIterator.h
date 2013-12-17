/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2FileInputIterator.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující vstupní iterátor ze souboru.
 *
 ******************************************************************************/

/**
 * @file MPEG2FileInputIterator.h
 *
 * @brief Module which implements file input iterator.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2FILEINPUTITERATOR_H
#define MPEG2FILEINPUTITERATOR_H

#include <iterator>

#include "MPEG2InputIterator.h"

using namespace std;

istream &operator>>( istream  &is, MPEG2Packet &packet );

/**
 * Class representing MPEG2 iterator from the file.
 */
class MPEG2FileInputIterator: public MPEG2InputIterator {
public:
    MPEG2FileInputIterator(istream_iterator<MPEG2Packet> mpeg2Iter);

    virtual MPEG2InputIterator& operator++() override;
    virtual const MPEG2Packet& operator*() const override;
    virtual const MPEG2Packet* operator->() const override;
    virtual bool operator==(const MPEG2InputIterator& rhs) const override;

protected:
    istream_iterator<MPEG2Packet> mpeg2Iter;
};

#endif // MPEG2FILEINPUTITERATOR_H
