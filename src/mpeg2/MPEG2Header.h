/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2Header.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající hlavičku MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2Header.h
 *
 * @brief Module which reads header of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2HEADER_H
#define MPEG2HEADER_H

#include <vector>

#include <cstdint>

/**
 * The Scrambling Control enum
 */
enum ScramblingControl {
    NotScrambled            = 0x00,
    ReservedForFutureUse    = 0x01,
    ScrambledWithEvenKey    = 0x03,
    ScrambledWithOddKey     = 0x04
};

/**
 * The Adaptation Field Control enum
 */
enum AdaptationFieldControl {
    NoAdaptationFields          = 0x01,
    AdaptationFieldOnly         = 0x02,
    AdaptationFieldAndPayload   = 0x03
};

/**
 * Class representing MPEG2 Header
 */
class MPEG2Header
{
public:
    static const uint8_t CONTINUITY_COUTER_SIZE   = 16;
    MPEG2Header(std::vector<uint8_t> &header);

    uint8_t synByte;
    bool transportErrorIndicator;
    bool payloadUnitStartIndicator;
    bool transportPriority;
    uint16_t PID;
    ScramblingControl scramblingControl;
    AdaptationFieldControl adaptationFieldControl;
    uint8_t continuityCounter;
};

#endif // MPEG2HEADER_H
