/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          Descriptors.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul obsahující deskriptory transportního streamu
 *
 ******************************************************************************/

/**
 * @file Descriptors.cpp
 *
 * @brief Module which contains descriptor processors of the transport stream.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "TimeOffsetTable.h"
#include "Descriptors.h"

using namespace std;

/**
 * Parses the descriptor from the data vector and constructs descriptor object.
 * @param data Vector with the desriptor.
 */
Descriptor::Descriptor(vector<uint8_t> &data) {
    if (data.size() < DESCRIPTOR_HEADER_SIZE) {
        throw runtime_error ("Descriptor have to containa at least header with decriptor tag and lehgth!");
    }

    tag = data[0];
    length = data[1];

    if (length + DESCRIPTOR_HEADER_SIZE > data.size()) {
        throw runtime_error ("Passed data vector is not sufficient for reading descriptor with length: " + to_string(length));
    }

    if (length > 0) {
        descriptorBody.insert(descriptorBody.end(), &data[2], &data[2 + length]);
    }

    totalLength = length + DESCRIPTOR_HEADER_SIZE;
}

/**
 * Parses the descriptor from the data vector and constructs descriptor object. Additionaly does tag checking.
 * @param data Vector with the desriptor.
 * @param descTag Tag of which should be the descriptor
 */
Descriptor::Descriptor(vector<uint8_t> &data, uint8_t descTag) : Descriptor(data) {
    if (tag != descTag) {
        throw runtime_error ("Invalid descriptor tag! Expected: " + to_string(descTag));
    }
}

/**
 * Constructs descriptor from the data vector.
 *
 * @param data Vector with the desriptor.
 * @return Pointer to the descriptor.
 */
shared_ptr<Descriptor> DescriptorFactory::readDescriptor(vector<uint8_t> &data) {
    shared_ptr<Descriptor> descriptor(new Descriptor(data));
    switch (descriptor->tag) {
    case NetworkNameDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new NetworkNameDescriptor(data));
    case TerrestialDeliverySystemDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new TerrestialDeliverySystemDescriptor(data));
    case ServiceDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new ServiceDescriptor(data));
    case ShortEventDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new ShortEventDescriptor(data));
    case ISO639LanguageDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new ISO639LanguageDescriptor(data));
    case LocalTimeOffsetDescriptor::DESCRIPTOR_TAG:
        return shared_ptr<Descriptor>(new LocalTimeOffsetDescriptor(data));

    }

    return descriptor;
}

/**
 * Constructs descriptor loop from the data vector.
 *
 * @param data Vector with the desriptors.
 * @return Descriptor loop.
 */
DescriptorLoop DescriptorFactory::readDescriptorLoop(vector<uint8_t> &data)
{
    DescriptorLoop loop;
    uint8_t headerSize = DescriptorLoop::DESCRIPTOR_LOOP_HEADER_SIZE;

    if (data.size() < headerSize) {
        throw runtime_error ("Unable to read descriptor loop, insufficient size of header!");
    }

    uint16_t descriptorsLength = (data[0] & 0x0F) << 8;
    descriptorsLength |= data[1];

    if (data.size() < descriptorsLength + headerSize) {
        throw runtime_error ("Unable to read descriptors, insufficient size of data vector!");
    }

    vector<uint8_t> descData(&data[headerSize], &data[headerSize + descriptorsLength]);
    while (!descData.empty()) {
        shared_ptr<Descriptor> descriptor = readDescriptor(descData);
        loop.descriptors.push_back(descriptor);
        descData.erase(descData.begin(), descData.begin() + descriptor->totalLength);
    }

    loop.totalLength = descriptorsLength + headerSize;

    return loop;
}

/**
 * Constructs specialized Network Name Descriptor
 * @param data Vector with the desriptor.
 */
NetworkNameDescriptor::NetworkNameDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {
    networkName = string((char *)&descriptorBody[0], descriptorBody.size());
}

/**
 * Constructs bandwith from the type.
 * @param type Type of the bandwidth
 */
Bandwidth::Bandwidth(BandwidthEnum type) : type(type) {}

/**
 * Converts bandwith to a string.
 * @return Bandwidth as a string
 */
string Bandwidth::toString() const {
    switch (type) {
        case BandwidthEnum::_8MHz:
        return "8 MHz";
        case BandwidthEnum::_7MHz:
        return "7 MHz";
        case BandwidthEnum::_6MHz:
        return "6 MHz";
        case BandwidthEnum::_5MHz:
        return "5 MHz";
    default:
        return "reserved";
    }
}

/**
 * Converts bandwidth to a double.
 * @return Bandwidth as double.
 */
double Bandwidth::toValue() const {
    switch (type) {
        case BandwidthEnum::_8MHz:
        return 8000000;
        case BandwidthEnum::_7MHz:
        return 7000000;
        case BandwidthEnum::_6MHz:
        return 6000000;
        case BandwidthEnum::_5MHz:
        return 5000000;
    default:
        return 0;
    }
}

/**
 * Constructs constellation from a type.
 * @param type Type of the constellation.
 */
Constellation::Constellation(ConstellationEnum type) : type(type) {}

/**
 * Converts constellation to a string.
 * @return Constellation as a string.
 */
string Constellation::toString() const {
    switch (type) {
        case ConstellationEnum::_QPSK:
        return "QPSK";
        case ConstellationEnum::_16QAM:
        return "16-QAM";
        case ConstellationEnum::_64QAM:
        return "64-QAM";
    default:
        return "reserved";
    }
}

/**
 * Converts constellation to a double.
 * @return Constelation as a double.
 */
double Constellation::toValue() const {
    switch (type) {
        case ConstellationEnum::_QPSK:
        return 2;
        case ConstellationEnum::_16QAM:
        return 4;
        case ConstellationEnum::_64QAM:
        return 6;
    default:
        return 0;
    }
}

/**
 * Constructs coderate from type.
 * @param type Type of the constelation.
 */
CodeRate::CodeRate(CodeRateEnum type) : type(type) {}

/**
 * Converests coderate to a string.
 * @return Coderate as a string.
 */
string CodeRate::toString() const {
    switch (type) {
        case CodeRateEnum::_1_2:
        return "1/2";
        case CodeRateEnum::_2_3:
        return "2/3";
        case CodeRateEnum::_3_4:
        return "3/4";
        case CodeRateEnum::_5_6:
        return "5/6";
        case CodeRateEnum::_7_8:
        return "7/8";
    default:
        return "reserved";
    }
}

/**
 * Constructs coderate to a double value.
 * @return Coderate as a double value.
 */
double CodeRate::toValue() const {
    switch (type) {
        case CodeRateEnum::_1_2:
        return 1.0/2;
        case CodeRateEnum::_2_3:
        return 2.0/3;
        case CodeRateEnum::_3_4:
        return 3.0/4;
        case CodeRateEnum::_5_6:
        return 5.0/6;
        case CodeRateEnum::_7_8:
        return 7.0/8;
    default:
        return 0;
    }
}

/**
 * Constructs guard interval from the type.
 * @param type Type of the guard interval.
 */
GuardInterval::GuardInterval(GuardIntervalEnum type) : type(type) {}

/**
 * Converts guard interval to a string.
 * @return Guard interval as a string.
 */
string GuardInterval::toString() const {
    switch (type) {
        case GuardIntervalEnum::_1_32:
        return "1/32";
        case GuardIntervalEnum::_1_16:
        return "1/16";
        case GuardIntervalEnum::_1_8:
        return "1/8";
        case GuardIntervalEnum::_1_4:
        return "1/4";
    default:
        return "undefined";
    }
}

/**
 * Converts guard interval to a double value.
 * @return Guard interval as a double value.
 */
double GuardInterval::toValue() const {
    switch (type) {
        case GuardIntervalEnum::_1_32:
        return 32.0/33;
        case GuardIntervalEnum::_1_16:
        return 16.0/17;
        case GuardIntervalEnum::_1_8:
        return 8.0/9;
        case GuardIntervalEnum::_1_4:
        return 4.0/5;
    default:
        return 0;
    }
}

/**
 * Constructs specialized Terrestial Delivery System Descriptor
 * @param data Vector with the desriptor.
 */
TerrestialDeliverySystemDescriptor::TerrestialDeliverySystemDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {

    if (descriptorBody.size() < DESCRIPTOR_BODY_SIZE) {
        throw runtime_error ("Passed data vector is not sufficient for reading descriptor with length: " + to_string(DESCRIPTOR_BODY_SIZE));
    }

    uint8_t *dataPtr = &descriptorBody[0];

    centreFrequency = *dataPtr++ << 24;
    centreFrequency |= *dataPtr++ << 16;
    centreFrequency |= *dataPtr++ << 8;
    centreFrequency |= *dataPtr++;

    bandwidth = Bandwidth((BandwidthEnum)((*dataPtr & 0xE0) >> 5));
    priority = *dataPtr++ & 0x10;

    constellation = Constellation((ConstellationEnum)((*dataPtr & 0xC0) >> 6));
    codeRateHP = CodeRate((CodeRateEnum)(*dataPtr++ & 0x07));

    codeRateLP = CodeRate((CodeRateEnum)((*dataPtr & 0xE0) >> 5));
    guardInterval = GuardInterval((GuardIntervalEnum)((*dataPtr & 0x18) >> 3));
}

/**
 * Constructs specialized Service Descriptor
 * @param data Vector with the desriptor.
 */
ServiceDescriptor::ServiceDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {

    uint16_t readLen = 0;
    if (descriptorBody.size() == 0) {
        throw runtime_error ("Passed data vector does not contain service_type item!");
    }
    serviceType = (ServiceType)descriptorBody[readLen];
    readLen++;

    if (descriptorBody.size() < (unsigned)(readLen + 1)) {
        throw runtime_error ("Passed data vector does not contain length of service_provider_name item!");
    }
    uint8_t len = descriptorBody[readLen];
    readLen++;

    if (descriptorBody.size() < len + readLen) {
        throw runtime_error ("Passed data vector does not contain service_provider_name item full length!");
    }
    serviceProviderName = string((char *)&descriptorBody[readLen], len);
    readLen += len;

    if (descriptorBody.size() < (unsigned)(readLen + 1)) {
        throw runtime_error ("Passed data vector does not contain length of service_provider_name item!");
    }
    len = descriptorBody[readLen];
    readLen++;

    if (descriptorBody.size() < len + readLen) {
        throw runtime_error ("Passed data vector does not contain service_provider_name item full length!");
    }
    serviceName = string((char *)&descriptorBody[readLen], len);
}

/**
 * Constructs specialized Short Event Descriptor
 * @param data Vector with the desriptor.
 */
ShortEventDescriptor::ShortEventDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {

    uint16_t readLen = 0;
    if (descriptorBody.size() < DESCRIPTOR_CODE_LANGUAGE_SIZE) {
        throw runtime_error ("Passed data vector does not contain code language item!");
    }
    languageCode =  string((char *)&descriptorBody[readLen], DESCRIPTOR_CODE_LANGUAGE_SIZE);
    readLen += DESCRIPTOR_CODE_LANGUAGE_SIZE;

    if (descriptorBody.size() < (unsigned)(readLen + 1)) {
        throw runtime_error ("Passed data vector does not contain length of event_name item!");
    }
    uint8_t len = descriptorBody[readLen];
    readLen++;

    if (descriptorBody.size() < len + readLen) {
        throw runtime_error ("Passed data vector does not contain full length of event_name!");
    }
    eventName = string((char *)&descriptorBody[readLen], len);
    readLen += len;

    if (descriptorBody.size() < (unsigned)(readLen + 1)) {
        throw runtime_error ("Passed data vector does not contain length of event_text item!");
    }
    len = descriptorBody[readLen];
    readLen++;

    if (descriptorBody.size() < len + readLen) {
        throw runtime_error ("Passed data vector does not contain full length of event_text!");
    }
    eventText = string((char *)&descriptorBody[readLen], len);
}

/**
 * Constructs specialized ISO 639 Language Descriptor
 * @param data Vector with the desriptor.
 */
ISO639LanguageDescriptor::ISO639LanguageDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {

    uint16_t readLen = 0;
    if (descriptorBody.size() < DESCRIPTOR_BODY_SIZE) {
        throw runtime_error ("Passed data vector does does not have correct size!");
    }
    languageCode =  string((char *)&descriptorBody[readLen], DESCRIPTOR_CODE_LANGUAGE_SIZE);
    readLen += DESCRIPTOR_CODE_LANGUAGE_SIZE;

    audioType = (AudioType)descriptorBody[readLen];
}

/**
 * Constructs specialized Local Time Offset Descriptor
 * @param data Vector with the desriptor.
 */
LocalTimeOffsetDescriptor::LocalTimeOffsetDescriptor(vector<uint8_t> &data)
    : Descriptor(data, DESCRIPTOR_TAG) {

    if (descriptorBody.size() < LTO_DESCRIPTOR_SIZE) {
        throw runtime_error ("Passed data vector have size at least 10!");
    }

    int readLength = 0;

    countryCode = descriptorBody[readLength] << 16;
    countryCode |= descriptorBody[readLength + 1] << 8;
    countryCode |= descriptorBody[readLength + 2];
    readLength += 3;

    regionID = descriptorBody[readLength] & 0xFC;
    timeOffsetPolarity = descriptorBody[readLength] & 0x01;
    readLength++;

    vector<uint8_t> timeData(&descriptorBody[readLength], &descriptorBody[descriptorBody.size()]);
    timeData.resize(2);
    timeData.push_back(0);
    timeOffset = DateTime::parseTime(timeData);
    readLength += 2;

    vector<uint8_t> dateTimeData(&descriptorBody[readLength], &descriptorBody[descriptorBody.size()]);
    timeOfChange = DateTime::parseDateTime(dateTimeData);
    readLength += 5;

    vector<uint8_t> timeData2(&descriptorBody[readLength], &descriptorBody[descriptorBody.size()]);
    timeData2.resize(2);
    timeData2.push_back(0);
    nextOffset = DateTime::parseTime(timeData2);
    readLength += 2;
 }

/**
 * Converts number to hex number which is stored again as a decimal number.
 * @param number Number to be converted.
 * @return Hexadecimal number represented by decimal.
 */
int DateTime::toHex(unsigned int number) {
    std::stringstream stream;
    stream << std::hex << number;
    std::string result( stream.str() );
    return std::stoi(result);
}

/**
 * Parses date and time from he vector.
 * @param data Vector with the date time.
 * @return Datetime from the vector.
 */
struct tm DateTime::parseDateTime(vector<uint8_t> &data) {
    if (data.size() < DATETIME_SIZE) {
        throw runtime_error ("Unable to read datetime, insufficient size!");
    }

    uint8_t *dataPtr = &data[0];

    struct tm resTime;
    int MJD = *dataPtr++ << 8;
    MJD |= *dataPtr++;

    int Y = (MJD - 15078.2) / 365.25;
    int M = (MJD - 14956.1 - (int)(Y * 365.25)) / 30.6001;
    resTime.tm_mday = MJD - 14956 - (int)(Y * 365.25) - (int)(M * 30.6001);
    int K = (M == 14 || M == 15)? 1 : 0;
    resTime.tm_year = Y + K;
    resTime.tm_mon = M - 2 - K * 12;

    resTime.tm_hour = toHex(*dataPtr++);
    resTime.tm_min = toHex(*dataPtr++);
    resTime.tm_sec = toHex(*dataPtr++);
    resTime.tm_isdst = -1;

    return resTime;
}

/**
 * Parses time from the vector.
 * @param data Vector with the time.
 * @return Time parsed from the vector.
 */
struct tm DateTime::parseTime(vector<uint8_t> &data) {
    if (data.size() < TIME_SIZE) {
        throw runtime_error ("Unable to read time, insufficient size!");
    }

    uint8_t *dataPtr = &data[0];
    struct tm resTime;

    resTime.tm_mday = 1;
    resTime.tm_year = 0;
    resTime.tm_mon = 0;
    resTime.tm_hour = toHex(*dataPtr++);
    resTime.tm_min = toHex(*dataPtr++);
    resTime.tm_sec = toHex(*dataPtr++);
    resTime.tm_wday = 0;
    resTime.tm_yday = 0;
    resTime.tm_isdst = -1;

    return resTime;
}

/**
 * Offsets datetime about passed offset time.
 * @param timeToOffset Datetime to be offeseted.
 * @param offsetTime Time about which to offset demanded datetime.
 * @return Offseted datetime.
 */
struct tm DateTime::offset(struct tm& timeToOffset, struct tm &offsetTime) {

    struct tm offsetedTime = timeToOffset;
    offsetedTime.tm_hour += offsetTime.tm_hour;
    offsetedTime.tm_min += offsetTime.tm_min;
    offsetedTime.tm_sec += offsetTime.tm_sec;

    mktime(&offsetedTime);

    return offsetedTime;
}
