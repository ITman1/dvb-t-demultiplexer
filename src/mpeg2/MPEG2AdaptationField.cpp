/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2AdaptationField.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající adaptující pole MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2AdaptationField.cpp
 *
 * @brief Module which reads adaptation field of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <stdexcept>

#include "MPEG2AdaptationField.h"

using namespace std;

/**
 * Reads adaptation fields from the data vector.
 * @param field Data vector with the adapation fields.
 */
MPEG2AdaptationField::MPEG2AdaptationField(vector<uint8_t> &field) :length(0)
{
    if (field.size() < ADAPTATION_FIELD_HEADER_SIZE) {
        throw runtime_error ("Unable to read header of Adaptation field, too small!");
    }

    length = field[0];
    length = (length > ADAPTATION_FIELD_MAXSIZE)? ADAPTATION_FIELD_MAXSIZE : length;

    discontinuityIndicator = field[1] & 0x80;
    randomAccessindicator = field[1] & 0x40;
    elementaryStreamPriorityIndicator = field[1] & 0x20;
    flags = (AdaptationFieldFlags)(field[1] & 0x1F);

    totalLength = length + 1;
}
