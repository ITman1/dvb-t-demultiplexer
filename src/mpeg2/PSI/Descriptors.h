/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          Descriptors.h
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul obsahující deskriptory transportního streamu
 *
 ******************************************************************************/

/**
 * @file Descriptors.h
 *
 * @brief Module which contains descriptor processors of the transport stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <memory>
#include <vector>
#include <string>

#include <ctime>
#include <cstdint>

using namespace std;

/**
 * Base class for all descriptors.
 */
class Descriptor {
protected:
    const unsigned int static DESCRIPTOR_HEADER_SIZE        = 2;

    vector<uint8_t> descriptorBody;
public:
    Descriptor(vector<uint8_t> &data);
    Descriptor(vector<uint8_t> &data, uint8_t descTag);
    Descriptor(uint8_t tag) : tag(tag) {}

    uint8_t tag;
    uint8_t length;
    uint32_t totalLength;
};

/**
 * Class representing vector of descriptors.
 */
class Descriptors : public vector<shared_ptr<Descriptor>> {
public:
    template <class SpecDescriptor>
    bool getSpecificDescriptor(SpecDescriptor &descriptor) const {
        vector<shared_ptr<Descriptor>>::const_iterator it = begin();
        for (; it != end(); it++) {
            if ((*it)->tag == descriptor.tag) {
                descriptor = *static_cast<SpecDescriptor*>(it->get());
                return true;
            }
        }
        return false;
    }
};

/**
 * Class representing loop of descriptors
 */
class DescriptorLoop {
public:
    DescriptorLoop() : totalLength(0) {}

    const unsigned int static DESCRIPTOR_LOOP_HEADER_SIZE        = 2;

    Descriptors descriptors;
    uint32_t totalLength;
};

/**
 * Class for constructing descriptors.
 */
class DescriptorFactory {
public:
    static shared_ptr<Descriptor> readDescriptor(vector<uint8_t> &data);
    static DescriptorLoop readDescriptorLoop(vector<uint8_t> &data);
};

/**
 * The Network Name Descriptor class
 */
class NetworkNameDescriptor : public Descriptor {
public:
    NetworkNameDescriptor(vector<uint8_t> &data);
    NetworkNameDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                = 0x40;

    string networkName;
};

/**
 * The Bandwidth enum
 */
enum BandwidthEnum {
    _8MHz       = 0x00,
    _7MHz       = 0x01,
    _6MHz       = 0x02,
    _5MHz       = 0x03
};

/**
 * The Bandwidth class
 */
class Bandwidth {
public:
    Bandwidth() {}
    Bandwidth(BandwidthEnum type);
    string toString() const;
    double toValue() const;

    BandwidthEnum type;
};

/**
 * The Constellation enum
 */
enum ConstellationEnum {
    _QPSK       = 0x00,
    _16QAM      = 0x01,
    _64QAM      = 0x02
};

/**
 * The Constellation class
 */
class Constellation {
public:
    Constellation() {}
    Constellation(ConstellationEnum type);

    string toString() const;
    double toValue() const;

    ConstellationEnum type;
};

/**
 * The Code Rate enum
 */
enum CodeRateEnum {
    _1_2       = 0x00,
    _2_3       = 0x01,
    _3_4       = 0x02,
    _5_6       = 0x03,
    _7_8       = 0x04
};

/**
 * The Code Rate class
 */
class CodeRate {
public:
    CodeRate() {}
    CodeRate(CodeRateEnum type);

    string toString() const;
    double toValue() const;

    CodeRateEnum type;
};

/**
 * The Guard Interval Enum enum
 */
enum GuardIntervalEnum {
    _1_32       = 0x00,
    _1_16       = 0x01,
    _1_8        = 0x02,
    _1_4        = 0x03
};

/**
 * The Guard Interval class
 */
class GuardInterval {
public:
    GuardInterval() {}
    GuardInterval(GuardIntervalEnum type);

    string toString() const;
    double toValue() const;

    GuardIntervalEnum type;
};

/**
 * The Terrestial Delivery System Descriptor class
 */
class TerrestialDeliverySystemDescriptor : public Descriptor {
protected:
    const unsigned int static DESCRIPTOR_BODY_SIZE        = 11;
public:
    TerrestialDeliverySystemDescriptor(vector<uint8_t> &data);
    TerrestialDeliverySystemDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                = 0x5A;

    uint32_t centreFrequency;
    Bandwidth bandwidth;
    bool priority;
    /*
     * TODO:
     *
     * bool timeSlicingIndicator;
     * bool MPE_FEC_Indicator;
     */

    Constellation constellation;
    /*
     * TODO:
     * HierarchyInformation hierarchyInformation;
     */
    CodeRate codeRateHP;
    CodeRate codeRateLP;
    GuardInterval guardInterval;
    /*
     * TODO:
     * TransmissionMode transmissionMode;
     * bool otherFrequencyFlag;
     */
};

/**
 * The Service Type enum
 */
enum ServiceType {
    DIGITAL_TV      = 0x01,
    DIGITAL_RADIO   = 0x02,
    TELETEXT        = 0x03
};

/**
 * The Service Descriptor class
 */
class ServiceDescriptor : public Descriptor {
public:
    ServiceDescriptor(vector<uint8_t> &data);
    ServiceDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                = 0x48;

    ServiceType serviceType;

    string serviceProviderName;
    string serviceName;
};

/**
 * The Short Event Descriptor class
 */
class ShortEventDescriptor : public Descriptor {
public:
    ShortEventDescriptor(vector<uint8_t> &data);
    ShortEventDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                  = 0x4D;
    const unsigned int static DESCRIPTOR_CODE_LANGUAGE_SIZE   = 3;

    string languageCode;
    string eventName;
    string eventText;
};

/**
 * The Audio Type enum
 */
enum AudioType {
    UNDEFINED   = 0x00,
    MAIN_AUDIO  = 0x01
};

/**
 * The ISO 639 Language Descriptor class
 */
class ISO639LanguageDescriptor : public Descriptor {
protected:
    const unsigned int static DESCRIPTOR_BODY_SIZE        = 4;
public:
    ISO639LanguageDescriptor(vector<uint8_t> &data);
    ISO639LanguageDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                  = 0x0a;
    const unsigned int static DESCRIPTOR_CODE_LANGUAGE_SIZE   = 3;

    string languageCode;
    AudioType audioType;
};

/**
 * The Local Time Offset Descriptor class
 */
class LocalTimeOffsetDescriptor : public Descriptor {
public:
    LocalTimeOffsetDescriptor(vector<uint8_t> &data);
    LocalTimeOffsetDescriptor() : Descriptor(DESCRIPTOR_TAG) {}

    const unsigned int static DESCRIPTOR_TAG                  = 0x58;
    const unsigned int static LTO_DESCRIPTOR_SIZE = 13;

    uint32_t countryCode;
    uint8_t regionID;
    bool timeOffsetPolarity;

    struct tm timeOffset;
    struct tm timeOfChange;
    struct tm nextOffset;
};

namespace DateTime {
    const static uint8_t DATETIME_SIZE = 5;
    const static uint8_t TIME_SIZE = 2;

    struct tm parseDateTime(vector<uint8_t> &data);
    struct tm parseTime(vector<uint8_t> &data);
    int toHex(unsigned int number);
    struct tm offset(struct tm& timeToOffset, struct tm &offsetTime);
}

#endif // DESCRIPTORS_H
