/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2InputIterator.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Hlavička obsahující základní třídu pro tvorbu MPEG2 vstupního iterátoru.
 *
 ******************************************************************************/

/**
 * @file MPEG2InputIterator.h
 *
 * @brief Header which contains base class for MPEG2 input iterator.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2INPUTITERATOR_H
#define MPEG2INPUTITERATOR_H

#include "../MPEG2Packet.h"

/**
 * Base class for all MPEG2 input iterators.
 */
class MPEG2InputIterator {
public:
    MPEG2InputIterator() {}
    virtual ~MPEG2InputIterator() {}

    virtual MPEG2InputIterator&  operator++() {
        return *this;
    }

    virtual const MPEG2Packet& operator*() const {
        return defaultPacket;
    }

    virtual const MPEG2Packet* operator->() const {
        return &defaultPacket;
    }

    virtual bool operator==(const MPEG2InputIterator& ) const {
        return false;
    }

    MPEG2InputIterator operator++(int) {
        MPEG2InputIterator tmp(*this);
        operator++();
        return tmp;
    }

    bool operator!=(const MPEG2InputIterator& rhs) {
        return !(*this == rhs);
    }

protected:
    mutable MPEG2Packet defaultPacket;
};

#endif // MPEG2INPUTITERATOR_H
