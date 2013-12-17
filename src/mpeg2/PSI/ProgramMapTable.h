/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ProgramMapTable.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro zpracovávání programové tabulky
 *
 ******************************************************************************/

/**
 * @file ProgramMapTable.h
 *
 * @brief Module for processing Program Map Table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef PROGRAMMAPTABLE_H
#define PROGRAMMAPTABLE_H

#include <vector>

#include "Descriptors.h"
#include "ServiceInformationTable.h"

using namespace std;

/**
 * Class representing program stream
 */
class ProgramStream {
protected:
    const unsigned int static STREAM_HEADER_SIZE        = 3;
public:
    typedef enum StreamType {
        BDA_UNITIALIZED_MPEG2STREAMTYPE  = -1,
        Reserved1                        = 0x00,
        ISO_IEC_11172_2_VIDEO            = 0x01,
        ISO_IEC_13818_2_VIDEO            = 0x02,
        ISO_IEC_11172_3_AUDIO            = 0x03,
        ISO_IEC_13818_3_AUDIO            = 0x04,
        ISO_IEC_13818_1_PRIVATE_SECTION  = 0x05,
        ISO_IEC_13818_1_PES              = 0x06,
        ISO_IEC_13522_MHEG               = 0x07,
        ANNEX_A_DSM_CC                   = 0x08,
        ITU_T_REC_H_222_1                = 0x09,
        ISO_IEC_13818_6_TYPE_A           = 0x0A,
        ISO_IEC_13818_6_TYPE_B           = 0x0B,
        ISO_IEC_13818_6_TYPE_C           = 0x0C,
        ISO_IEC_13818_6_TYPE_D           = 0x0D,
        ISO_IEC_13818_1_AUXILIARY        = 0x0E,
        ISO_IEC_13818_1_RESERVED         = 0x0F,
        USER_PRIVATE                     = 0x10,
        ISO_IEC_USER_PRIVATE             = 0x80,
        DOLBY_AC3_AUDIO                  = 0x81
    } StreamType;

    ProgramStream(vector<uint8_t> &data);

    StreamType streamType;
    uint16_t elementaryPID;

    Descriptors ESDescriptors;

    uint16_t totalLength;
};

/**
 * Class representing program map table
 */
class ProgramMapTable
{
protected:
    const unsigned int static PMT_HEADER_SIZE        = 7;
    const uint8_t static PMT_TABLE_ID                = 0x02;
public:
    ProgramMapTable(ServiceInformationTable &table);
    ProgramMapTable() {}

    static shared_ptr<ProgramMapTable> fromPacketStream(MPEG2InputStream &stream, uint16_t pid);

    uint16_t tableID;
    uint16_t tablePID;

    uint16_t programNumber;
    uint8_t versionNumber;
    bool currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    uint16_t PCR_PID;
    Descriptors programDescriptors;
    vector<ProgramStream> streams;
};

#endif // PROGRAMMAPTABLE_H
