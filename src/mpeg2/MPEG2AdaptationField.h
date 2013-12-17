/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2AdaptationField.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul zpracovávající adaptující pole MPEG2 packetu.
 *
 ******************************************************************************/

/**
 * @file MPEG2AdaptationField.h
 *
 * @brief Module which reads adaptation field of the MPEG2 packet.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef MPEG2ADAPTATIONFIELD_H
#define MPEG2ADAPTATIONFIELD_H

#include <vector>

#include <cstdint>

/**
 * The Adaptation Field Flags enum
 */
enum AdaptationFieldFlags {
    PCR                         = 0x01,
    OPCR                        = 0x02,
    SplicingPoint               = 0x04,
    TransportPrivateData        = 0x08,
    AdaptationFieldExtension    = 0x10
};

/**
 * Class representing adaptation fields of MPEG2 packet
 */
class MPEG2AdaptationField
{
protected:
    const unsigned int static ADAPTATION_FIELD_HEADER_SIZE          = 2;
    const unsigned int static ADAPTATION_FIELD_MAXSIZE              = 183;
public:
    MPEG2AdaptationField(std::vector<uint8_t> &field);

    uint8_t length;
    bool discontinuityIndicator;
    bool randomAccessindicator;
    bool elementaryStreamPriorityIndicator;
    AdaptationFieldFlags flags;

    uint16_t totalLength;
};

#endif // MPEG2ADAPTATIONFIELD_H
