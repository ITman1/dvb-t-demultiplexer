/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2FileInputStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující vstupní stream ze souboru.
 *
 ******************************************************************************/

/**
 * @file MPEG2FileInputStream.cpp
 *
 * @brief Module which implements file input stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "MPEG2FileInputStream.h"

using namespace std;

/**
 * Constructs file input stream
 */
MPEG2FileInputStream::MPEG2FileInputStream()
    : endInputIter(MPEG2FileInputIterator(istream_iterator<MPEG2Packet>())) {}

/**
 * Resets stream to the beginning
 */
void MPEG2FileInputStream::reset() {
    clear();
    seekg(0, ios::beg) ;
    currInputFileIter = shared_ptr<MPEG2FileInputIterator>(new MPEG2FileInputIterator(istream_iterator<MPEG2Packet>(*this)));
}

/**
 * Returns current iterator
 * @return Current iterator
 */
MPEG2InputStream::iterator & MPEG2FileInputStream::current() {
    if (!currInputFileIter) {
        currInputFileIter = shared_ptr<MPEG2FileInputIterator>(new MPEG2FileInputIterator(istream_iterator<MPEG2Packet>(*this)));
    }
    return *currInputFileIter;
}

/**
 * Returns iterator which points to the end
 * @return Iterator which points to the end
 */
MPEG2InputStream::iterator & MPEG2FileInputStream::end() {
    return endInputIter;
}

/**
 * Returns number of the packet from the beginning
 * @return Number of the packet from the beginning
 */
long MPEG2FileInputStream::currentFrameNo() {
    return tellg() / MPEG2Packet::PACKET_SIZE;
}

/**
 * Reads MPEG2 packet from the file.
 * @param packet New packet from the input stream.
 * @return Reference to the current stream.
 */
MPEG2InputStream & MPEG2FileInputStream::operator>>( MPEG2Packet &packet ) {
    *this >> packet;
    return *this;
}
