/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2FileInputIterator.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující vstupní iterátor ze souboru.
 *
 ******************************************************************************/

/**
 * @file MPEG2FileInputIterator.cpp
 *
 * @brief Module which implements file input iterator.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include "MPEG2FileInputIterator.h"

using namespace std;

/**
 * Buffer for storing MPEG2 packets.
 */
static vector<uint8_t> __MPEG2FileInputIterator_buffer(MPEG2Packet::PACKET_SIZE);

/**
 * Reads MPEG2 packet from the file.
 * @param is Input stream from which is read MPEG2 packet.
 * @param packet New packet from the input stream.
 * @return Reference to the current stream.
 */
istream &operator>>( istream  &is, MPEG2Packet &packet ) {

    is.read((char *)&__MPEG2FileInputIterator_buffer[0], MPEG2Packet::PACKET_SIZE);
    if (is) {
        packet = MPEG2Packet(__MPEG2FileInputIterator_buffer);
    }
    return is;
}

/**
 * Constructs new file intput iterator from the input stream iterator.
 * @param mpeg2Iter Input stream iterator.
 */
MPEG2FileInputIterator::MPEG2FileInputIterator(istream_iterator<MPEG2Packet> mpeg2Iter)
    : mpeg2Iter(mpeg2Iter) {}

/**
 * Incremants the file input iterator.
 * @return Returns value of the new iterator.
 */
MPEG2InputIterator& MPEG2FileInputIterator::operator++() {
    mpeg2Iter++;
    return *this;
}

/**
 * Dereferences current value.
 * @return Current value where iterator points.
 */
const MPEG2Packet& MPEG2FileInputIterator::operator*() const {
    return *mpeg2Iter;
}

/**
 * Returns pointer to the value, where iterator points.
 * @return Pointer to the current value.
 */
const MPEG2Packet* MPEG2FileInputIterator::operator->() const {
    return &(*mpeg2Iter);
}

/**
 * Test for equal.
 * @param rhs Second iterator against which to compare.
 * @return True if both iterators are the same, otherwise false.
 */
bool MPEG2FileInputIterator::operator==(const MPEG2InputIterator& rhs) const {
    if (typeid(*this) == typeid(rhs)) {
        const MPEG2InputIterator *prhs = &rhs;
        return mpeg2Iter == static_cast<const MPEG2FileInputIterator *>(prhs)->mpeg2Iter;
    }

    return false;
}
