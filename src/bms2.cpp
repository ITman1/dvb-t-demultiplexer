/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          bms2.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Hlavní modul - získává informace z transportního streamu a
 *                  hieararchicky je ukládá do souborů
 *
 ******************************************************************************/

/**
 * @file bms2.cpp
 *
 * @brief Main (executable) module, collects informations from the transport
 * stream and saves these informations hierarchicaly into files.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <ctime>
#include <string>
#include <algorithm>
#include <memory>
#include <ios>
#include <map>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <set>

#include "mpeg2/PSI/ProgramAssociationTable.h"
#include "mpeg2/PSI/NetworkInformationTable.h"
#include "mpeg2/PSI/ServiceDescriptionTable.h"
#include "mpeg2/PSI/TimeOffsetTable.h"
#include "mpeg2/PSI/EventInformationTable.h"
#include "mpeg2/PSI/ProgramMapTable.h"
#include "mpeg2/PES/PacketElementaryStream.h"
#include "mpeg2/streams/MPEG2VideoFileStream.h"
#include "mpeg2/streams/MPEG2AudioFileStream.h"
#include "mpeg2/streams/MPEG2FileInputStream.h"
#include "miscellaneous.h"

using namespace std;

/*
 * Initializes macro which tries to read from the stream PSI table repeatly.
 *
 * @param name Unique name for macro statement.
 * @param is Input stream from which to read.
 * @param limit Maximum number of reading from the stream, if -1 is passed then it is tried to read until end of stream is reached.
 * @param reset_flag When true, then stream is firstly reseted, otherwise continues from the current position.
 */
#define RECOVERABLE_MPEG2IS_READ_INIT(name, is, limit, reset_flag)                \
    long _ ## name ## _tries = limit;                                                     \
    bool _ ## name ## _reset = reset_flag;                                                 \
    if (reset_flag) {                                                             \
        is.reset();                                                              \
    }

/*
 * Body of macro which tries to read from the stream PSI table repeatly.
 *
 * @param name Unique name for macro statement, the same as in the initialization.
 * @param table Table to which to store the PSI table.
 * @param is Input stream from which to read.
 */
#define RECOVERABLE_MPEG2IS_READ_BEGIN2(name, table, is)                                \
    try {                                                                        \
        while (is.current() != is.end()) {                                       \
            is.readPSITable(table, _ ## name ## _reset);                                   \
            _ ## name ## _reset = false;                                                  \
            if (!table) {                                                         \
                _ ## name ## _tries = (_ ## name ## _tries >= 0)? _ ## name ## _tries - 1 : _ ## name ## _tries;     \
            } else {                                                             \
                break;                                                           \
            }                                                                    \
        }

/*
 * Body of macro which tries to read from the stream PSI table repeatly.
 *
 * @param name Unique name for macro statement, the same as in the initialization.
 * @param table Table to which to store the PSI table.
 * @param is Input stream from which to read.
 * @param PID PID of the transport stream with the PSI table.
 */
#define RECOVERABLE_MPEG2IS_READ_BEGIN3(name, table, is, PID)                           \
    try {                                                                        \
        while (is.current() != is.end()) {                                       \
            is.readPSITable(table, PID, _ ## name ## _reset);                              \
            _ ## name ## _reset = false;                                                  \
            if (!table) {                                                         \
                _ ## name ## _tries = (_ ## name ## _tries >= 0)? _ ## name ## _tries - 1 : _ ## name ## _tries;     \
            } else {                                                             \
                break;                                                           \
            }                                                                    \
        }

/*
 * Finalization of macro which tries to read from the stream PSI table repeatly.
 *
 * @param name Unique name for macro statement, the same as in the initialization.
 * @param table Table to which to store the PSI table.
 * @param is Input stream from which to read.
 * @param err_msg Error message which is printed to stderr, when reading of table fails.
 */
#define RECOVERABLE_MPEG2IS_READ_END(name, table, is, err_msg)                          \
    } catch (const exception& error) {                                       \
        table.reset();                                                            \
        cerr << "Packet " << is.currentFrameNo() << ": " << err_msg << endl;                                                 \
        cerr << "Reason: " << error.what() << endl;                              \
                                                                                 \
        /* skipping current packet - it may cause this error */                  \
        if (is.current() != is.end()) {                                          \
           is.current()++;                                                       \
        }                                                                        \
        _ ## name ## _tries = (_ ## name ## _tries >= 0)? _ ## name ## _tries - 1 : _ ## name ## _tries;             \
        /* limit of maximum number of tries has been reached */                  \
        if (_ ## name ## _tries == 0) {                                                   \
            throw runtime_error("Reached maximum number of exception failures"); \
        }                                                                        \
    }

/**
 * Size of the auxilary buffer.
 */
const static int BUFFER_SIZE = 80;

/**
 * Auxilary buffer.
 */
static char BUFFER[BUFFER_SIZE];

/**
 * Gathers all necessary tables of the stream used for this application
 */
struct PSITables {
    shared_ptr<ProgramAssociationTable> PAT;
    shared_ptr<NetworkInformationTable> NIT;
    shared_ptr<ServiceDescriptionTable> SDT;
    shared_ptr<TimeOffsetTable> TOT;
    vector<ProgramMapTable> PMTs;
    vector<EventInformationTable> EITs;
};

/**
 * Stores necessary informations for the service.
 */
struct ServiceInfo {
    uint16_t PID;
    bool isVideo;
};

/**
 * Stores necessary informations about the event
 */
struct EventInfo {
    uint16_t id;
    struct tm dateTime;
    struct tm duration;
    string eventName;
    string eventText;

    /**
     * Compares two event structures by its ID and time
     * @param rhs Second event used for the comparison
     * @return True if this event is lesser than the passed event
     */
    bool operator< (const EventInfo& rhs) const {
        time_t t1 = mktime(const_cast<tm *>(&dateTime));
        time_t t2 = mktime(const_cast<tm *>(&rhs.dateTime));
        return id < rhs.id && t1 < t2;
    }
};

/**
 * Stores necessary informations about the channel program
 */
struct ProgramInfo {
    int PID;
    string serviceProvider;
    string serviceName;
    vector<ServiceInfo> services;
    vector<EventInfo> present;
    vector<EventInfo> schedule;
};

/**
 * Stores necessary informations about delivery method and its delivery parameters
 */
struct TerrestialDeliveryInfo {
    Bandwidth bandwidth;
    Constellation constellation;
    GuardInterval guardinterval;
    CodeRate codeRate;
};

/**
 * Stores necessary informations about the multiplex
 */
struct MultiplexInfo {
    string fileName;
    string networkName;
    int networkID;
    shared_ptr<TerrestialDeliveryInfo> delivery;
    vector<ProgramInfo> programs;

    MultiplexInfo() :
        networkName("(unknown)"), networkID(-1)
    {}
};

/**
 * Reads PSI tables from the stream.
 * @param is Input stream with the MPEG2 packets.
 * @param tables Tables which were read.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int readPSITables(MPEG2FileInputStream &is, PSITables &tables) {
    static const int MAXNUMBER_OF_FAILURES = 100;

    /* Read PAT */
    RECOVERABLE_MPEG2IS_READ_INIT(PAT, is, MAXNUMBER_OF_FAILURES, true);
    RECOVERABLE_MPEG2IS_READ_BEGIN2(PAT, tables.PAT, is);
        if (!tables.PAT) {
            cerr << "Unable to locate mandatory PAT table in the transport stream!" << endl;
            cerr << "Terminating application now due to previous error!" << endl;
            return EXIT_FAILURE;
        }
    RECOVERABLE_MPEG2IS_READ_END(PAT, tables.PAT, is, "Failed to read PAT table due to some internal error!");

    /* Retrieve PID of NIT */
    uint16_t pidNIT = NetworkInformationTable::NIT_DEFAULT_PID;
    for (const Program &program : tables.PAT->programs) {
        if (program.programNum == Program::NIT_PROG_NUM) {
            pidNIT = program.programPID;
            break;
        }
    }

    /* Read NIT */
    RECOVERABLE_MPEG2IS_READ_INIT(NIT, is, MAXNUMBER_OF_FAILURES, true);
    RECOVERABLE_MPEG2IS_READ_BEGIN3(NIT, tables.NIT, is, pidNIT);
        if (!tables.NIT) {
            cerr << "Unable to locate SDT table in the transport stream!" << endl;
        }
    RECOVERABLE_MPEG2IS_READ_END(NIT, tables.NIT, is, "Failed to read NIT table due to some internal error!");

    /* Read SDT */
    RECOVERABLE_MPEG2IS_READ_INIT(SDT, is, MAXNUMBER_OF_FAILURES, true);
    RECOVERABLE_MPEG2IS_READ_BEGIN2(SDT, tables.SDT, is);
        if (!tables.SDT) {
            cerr << "Unable to locate SDT table in the transport stream!" << endl;
        }
    RECOVERABLE_MPEG2IS_READ_END(SDT, tables.SDT, is, "Failed to read PMT table due to some internal error!");


    /* Read TOT */
    shared_ptr<TimeOffsetTable> TOT;
    RECOVERABLE_MPEG2IS_READ_INIT(TOT, is, -1, true);
    while (is.current() != is.end()) {
        RECOVERABLE_MPEG2IS_READ_BEGIN2(TOT, TOT, is);
        LocalTimeOffsetDescriptor ltod;
        /* Read TOT which contains offset information */
        if (TOT && TOT->descriptors.getSpecificDescriptor(ltod)) {
            tables.TOT = TOT;
            break;
        } else if (!TOT) {
            cerr << "Unable to locate TOT table!" << endl;
            continue;
        }
        RECOVERABLE_MPEG2IS_READ_END(TOT, TOT, is, "Failed to read TOT table due to some internal error!");
    }

    /* Read PMT tables */
    for (const Program &program : tables.PAT->programs) {
        if (program.programNum != Program::NIT_PROG_NUM) {
            shared_ptr<ProgramMapTable> PMT;
            RECOVERABLE_MPEG2IS_READ_INIT(PMT, is, MAXNUMBER_OF_FAILURES, true);
            RECOVERABLE_MPEG2IS_READ_BEGIN3(PMT, PMT, is, program.programPID);
                if (PMT) {
                    tables.PMTs.push_back(*PMT);
                } else {
                    cerr << "Unable to locate PMT table with PID " << hex << program.programPID << "!" << endl;
                    continue;
                }
            RECOVERABLE_MPEG2IS_READ_END(PMT, PMT, is, "Failed to read PMT table due to some internal error!");
        }
    }

    /* Read EITs */
    shared_ptr<EventInformationTable> EIT;
    RECOVERABLE_MPEG2IS_READ_INIT(EIT, is, -1, true);
    while (is.current() != is.end()) {
        RECOVERABLE_MPEG2IS_READ_BEGIN2(EIT, EIT, is);
            if (EIT) {
                tables.EITs.push_back(*EIT);
            }
        RECOVERABLE_MPEG2IS_READ_END(EIT, EIT, is, "Failed to read EIT table due to some internal error!");
    }

    return EXIT_SUCCESS;
}

/**
 * Transforms stream string into ASCII string
 * @param streamString Stream string to be transformed.
 * @return ASCII string
 */
string toAsciiString(string &streamString) {
    string asciiString(streamString);

    asciiString.erase(remove_if(asciiString.begin(), asciiString.end(), [] (const char &character) {
        return character >= (char)0xC0 && character <= (char)0xCF;
    }), asciiString.end());

    return asciiString;
}

/**
 * Fills vector with the events loaded from the tables
 *
 * @param tables Tables which were processed.
 * @param fromTableID Start ID of the event table from which read the events.
 * @param toTableID End ID of the event table from which read the events.
 * @param serviceID ID of the service with the demanded events.
 * @param eventInfos Output vector with events.
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int fillEventInfoVector(PSITables &tables, unsigned fromTableID, unsigned toTableID, unsigned serviceID, vector<EventInfo> &eventInfos) {
    set<EventInfo> info_set;
    for (const EventInformationTable &EIT : tables.EITs) {

        /* Continue if table ID and service ID does not match */
        if (EIT.tableID < fromTableID ||
                EIT.tableID > toTableID || EIT.serviceID != serviceID) {
            continue;
        }

        /* Process all events in the table and store them into the set */
        for (const Event &event : EIT.events) {
            EventInfo eventInfo;

            eventInfo.id = event.eventID;
            eventInfo.dateTime = event.startTime;
            eventInfo.duration = event.duration;

            /* Transfer start time about the local offset - if is available */
            struct tm startTimeCopy = event.startTime;
            time_t startTime = mktime(&startTimeCopy);
            LocalTimeOffsetDescriptor ltod;
            if (tables.TOT && tables.TOT->descriptors.getSpecificDescriptor(ltod)) {
                time_t timeOfChange = mktime(&ltod.timeOfChange);
                if (startTime < timeOfChange) {
                    eventInfo.dateTime = DateTime::offset(eventInfo.dateTime, ltod.timeOffset);
                } else {
                    eventInfo.dateTime = DateTime::offset(eventInfo.dateTime, ltod.nextOffset);
                }
            }

            /* Read description of the event */
            ShortEventDescriptor eventDescriptor;
            if (event.descriptors.getSpecificDescriptor(eventDescriptor)) {
                eventInfo.eventName = toAsciiString(eventDescriptor.eventName);
                eventInfo.eventText = toAsciiString(eventDescriptor.eventText);
            } else {
                cerr << "Failed to get ShortEventDescriptor for actual event of service " << hex << serviceID << " with event ID " << event.eventID  << "!" << endl;
            }

            /* Erase the old record in the set */
            info_set.erase(eventInfo);
            info_set.insert(eventInfo);
        }
    }

    /* Converts set into output vector */
    eventInfos.resize(info_set.size());
    copy(info_set.begin(), info_set.end(), eventInfos.begin());

    return EXIT_SUCCESS;
}

/**
 * Reads multiplex info from PSI tables into multiplex info structure
 * @param tables PSI tables
 * @param multInfo Multiplex info structure
 * @return EXIT_SUCCESS on success, otherwise EXIT_FAILURE
 */
int getMultiplexInfo(PSITables &tables, MultiplexInfo &multInfo) {
    /* Read informations from NIT table */
    if (tables.NIT) {
        NetworkInformationTable &NIT = *tables.NIT;

        multInfo.networkID = NIT.networkID;

        /* Get network name and delivery method */

        NetworkNameDescriptor networkDescriptor;
        if (!NIT.descriptors.getSpecificDescriptor(networkDescriptor)) {
            cerr << "Failed to get informations from NetworkNameDescriptor of NIT! NetworkNameDescriptor is not present!" << endl;
        } else {
            multInfo.networkName = networkDescriptor.networkName;

            /* Get the delivery method and parameters */
            TerrestialDeliverySystemDescriptor terDSD;
            bool loaded = false;
            for (const TransportStream &stream : NIT.streams) {
                if (stream.descriptors.getSpecificDescriptor(terDSD)) {
                    loaded = true;
                    break;
                }
            }

            /* Store delivery method into multiplex structure */
            if (!loaded) {
                cerr << "Failed to get informations from TerrestialDeliverySystemDescriptor of NIT! TerrestialDeliverySystemDescriptor is not present!" << endl;
            } else {
                multInfo.delivery = shared_ptr<TerrestialDeliveryInfo>(new TerrestialDeliveryInfo());
                multInfo.delivery->bandwidth = terDSD.bandwidth;
                if(terDSD.priority) {
                    multInfo.delivery->codeRate = terDSD.codeRateHP;
                } else {
                    multInfo.delivery->codeRate = terDSD.codeRateLP;
                }

                multInfo.delivery->constellation = terDSD.constellation;
                multInfo.delivery->guardinterval = terDSD.guardInterval;
            }
        }
    } else {
        cerr << "Failed to get informations from NIT! NIT is not present!" << endl;
    }

    /* Reads PMT tables and collect all necessary information for every station */
    for (const ProgramMapTable &currPMT : tables.PMTs) {
        ProgramInfo progInfo;
        progInfo.PID = currPMT.tablePID; // PID which carries the PMT table

        /* Locate corresponding service in the SDT table */
        vector<Service> &services = tables.SDT->services;
        vector<Service>::iterator serviceIter = find_if(services.begin(), services.end(), [&currPMT] (const Service &service) {
            return service.serviceID == currPMT.programNumber;
        });

        /* Service not found, skip this station */
        if (serviceIter == services.end()) {
            cerr << "Failed to get corresponding service from SDT for service ID in the current PMT!" << endl;
            cerr << "Channel with program number" << hex << currPMT.programNumber << " will  be skipped in the futher processing!" << endl;
            continue;
        }

        /* Service found, read some additional informations */
        ServiceDescriptor serviceDescriptor;
        if (!serviceIter->descriptors.getSpecificDescriptor(serviceDescriptor)) {
            cerr << "Failed to get corresponding ServiceDescriptor from actual service of SDT!" << endl;
            cerr << "Channel with program number" << hex << currPMT.programNumber << " will  be skipped in the futher processing!" << endl;
            continue;
        }

        /* Store addtional informations into program info structure */
        progInfo.serviceName = serviceDescriptor.serviceName;
        progInfo.serviceProvider = serviceDescriptor.serviceProviderName;

        /* Continue only if we are reading the digital television */
        if (serviceDescriptor.serviceType == ServiceType::DIGITAL_TV) {
            /* Read present events. */
            if (fillEventInfoVector(tables, EventInformationTable::EIT_PRESENT_TABLE_ID, EventInformationTable::EIT_PRESENT_TABLE_ID, currPMT.programNumber, progInfo.present) != EXIT_SUCCESS) {
                cerr << "Failed to read present events for channel with program number " << currPMT.programNumber << "!" << endl;
            }

            /* Read scheduled events. */
            if (fillEventInfoVector(tables, EventInformationTable::EIT_SCHEDULE_STARTTABLE_ID, EventInformationTable::EIT_SCHEDULE_ENDTABLE_ID, currPMT.programNumber, progInfo.schedule) != EXIT_SUCCESS) {
                cerr << "Failed to future events for channel with program number " << currPMT.programNumber << "!" << endl;
            }

            /* Get PID of video and audio streams */
            for (const ProgramStream &transportStream : currPMT.streams) {
                ServiceInfo serviceInfo;
                serviceInfo.PID = transportStream.elementaryPID;

                ISO639LanguageDescriptor languageDescriptor;
                switch (transportStream.streamType) {
                case ProgramStream::StreamType::ISO_IEC_11172_2_VIDEO:
                case ProgramStream::StreamType::ISO_IEC_13818_2_VIDEO:
                    serviceInfo.isVideo = true;
                    progInfo.services.push_back(serviceInfo);
                    break;
                case ProgramStream::StreamType::ISO_IEC_11172_3_AUDIO:
                case ProgramStream::StreamType::ISO_IEC_13818_3_AUDIO:
                    serviceInfo.isVideo = false;
                    if (transportStream.ESDescriptors.getSpecificDescriptor(languageDescriptor)) {
                        if (languageDescriptor.audioType == AudioType::MAIN_AUDIO || languageDescriptor.audioType == AudioType::UNDEFINED) {
                            progInfo.services.push_back(serviceInfo);
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            // Whole program has been successfuly read, store it into vector
            multInfo.programs.push_back(progInfo);
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Saves events into file
 * @param filename Filename of the output file
 * @param events Events to be saved into file
 * @return  0 on success, 1 on failure
 */
int saveEvents(string filename, const vector<EventInfo> &events) {
    /* Opens output file for storing the events. */
    ofstream output;
    output.open(filename);
    if( !output ) {
        cerr << "Unable to open file \"" <<  filename <<  "\" for writing events!" << endl;
        return EXIT_FAILURE;
    }

    /* Sorts events before saving. */
    vector<EventInfo> eventsCopy(events);
    sort(eventsCopy.begin(), eventsCopy.end(), [] (const EventInfo& lhs, const EventInfo& rhs) {
        time_t t1 = mktime(const_cast<tm *>(&lhs.dateTime));
        time_t t2 = mktime(const_cast<tm *>(&rhs.dateTime));
        return t1 < t2;
    });

    /* Save the events in the demanded format. */
    for (const EventInfo &event : eventsCopy) {
        strftime (BUFFER, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S - ", &event.dateTime);
        output << BUFFER << event.eventName << " - " << event.eventText << " - ";
        strftime (BUFFER, BUFFER_SIZE, "(%H:%M:%S)", &event.duration);
        output << BUFFER << endl;
    }
    output.close();

    return EXIT_SUCCESS;
}

/**
 * Saves multiplex info into output files and directories
 * @param is Input stream with MPEG2 packets
 * @param multInfo Parsed informations.
 * @return 0 on success, 1 on failure
 */
int saveMultiplexInfo(MPEG2FileInputStream &is, MultiplexInfo &multInfo) {
    /* Create output directory */
    if(createDirectory(multInfo.fileName.c_str()) > 0) {
        cerr << "Unable to create root directory \"" <<  multInfo.fileName <<  "\" for writing multiplex info!" << endl;
        return EXIT_FAILURE;
    }

    /* Open info.txt */

    ofstream infoOutput;
    string infoFilename = multInfo.fileName + string("/info.txt");
    infoOutput.open( infoFilename );

    if( !infoOutput ) {
        cerr << "Unable to create file \"" <<  infoFilename <<  "\" for writing multiplex info!" << endl;
    }

    /* Writing into info.txt */
    else {
        infoOutput << "Network name: " << multInfo.networkName << endl;
        infoOutput << "Network ID: " << multInfo.networkID << endl;
        infoOutput << "Bandwidth: " << ((multInfo.delivery)? multInfo.delivery->bandwidth.toString() : "(unknown)") << endl;
        infoOutput << "Constellation: " << ((multInfo.delivery)?multInfo.delivery->constellation.toString() : "(unknown)") << endl;
        infoOutput << "Guard interval: " << ((multInfo.delivery)?multInfo.delivery->guardinterval.toString() : "(unknown)") << endl;
        infoOutput << "Code rate: " <<((multInfo.delivery)? multInfo.delivery->codeRate.toString() : "(unknown)") << endl;
        infoOutput << endl;
    }

    /* Create program guide files and files for storing video and audio */

    map<uint16_t, shared_ptr<PacketStream> > streamsMap;
    for (const ProgramInfo &programInfo : multInfo.programs) {

        /* Construct folder name */
        stringstream programFolderStream;
        programFolderStream << multInfo.fileName + string("/");
        programFolderStream << "0x" << hex << setfill('0') << setw(4) << programInfo.PID;
        programFolderStream << "-" + programInfo.serviceProvider + "-" + programInfo.serviceName;
        string programFolder = programFolderStream.str();

        /* Create directory for program */
        if(createDirectory(programFolder.c_str()) > 0) {
            cerr << "Unable to create folder \"" <<  programFolder <<  "\" for channel with service name: " << programInfo.serviceName << "!" << endl;
            break;
        }

        /* Save present programs */
        if(saveEvents(programFolder + string("/epg-present.txt"), programInfo.present) != EXIT_SUCCESS) {
            cerr << "Unable to create file \"" <<  programFolder + string("/epg-present.txt") <<  "\" for saving present events!" << endl;
        }

        /* Save scheduled programs */
        if(saveEvents(programFolder + string("/epg-schedule.txt"), programInfo.schedule) != EXIT_SUCCESS) {
            cerr << "Unable to create file \"" <<  programFolder + string("/epg-schedule.txt") <<  "\" for saving schedule events!" << endl;
        }

        /* Open streams for video and audio */
        for (const ServiceInfo &serviceInfo : programInfo.services) {
            shared_ptr<MPEG2ServiceStream> serviceStream;
            string filename;

            /* Determine stream type and create correspondig stream to it */

            if (serviceInfo.isVideo) {
                serviceStream = shared_ptr<MPEG2ServiceStream>(new MPEG2VideoFileStream(serviceInfo.PID));
                filename = "video.m2v";
            } else {
                serviceStream = shared_ptr<MPEG2ServiceStream>(new MPEG2AudioFileStream(serviceInfo.PID));
                filename = "audio.wav";
            }

            /* Open stream and store it into streams map */

            string streamFileName = programFolder + string("/") + filename;
            serviceStream->open(streamFileName);

            if( !serviceStream ) {
                 cerr << "Unable to create stream file \"" <<  programFolder + string("/") + filename << endl;
                 continue;
            }

            streamsMap.insert(pair<uint16_t, shared_ptr<MPEG2ServiceStream> >(serviceInfo.PID, serviceStream));
        }
    }

    /* Process whole file and push transport streams into corresponding packets streams */
    is.reset();
    for (MPEG2FileInputStream::iterator &it = is.current(); it != is.end(); ++it) {
        const MPEG2Packet &packet = *it;
        map<uint16_t, shared_ptr<PacketStream> >::iterator streamIter = streamsMap.find(packet.header->PID);

        /* Get packet stream from map, or create it if does not exist! */
        shared_ptr<PacketStream> packetStream;
        if(streamIter != streamsMap.end())
        {
           packetStream = streamIter->second;
        } else {
            packetStream = shared_ptr<PacketStream>(new PacketStream(packet.header->PID));
            streamsMap.insert(pair<uint16_t, shared_ptr<PacketStream> >(packet.header->PID, packetStream));
        }

        /* Add current stream packet to the stream to which it belongs. */
        try {
            *packetStream << packet;
        } catch (const runtime_error& error) {
            cerr << "Packet " << is.currentFrameNo() << ": Internal error occured when reading MPEG2 packet!" << endl;
            cerr << "Reason: " << error.what() << endl;                                                                                                                           \
        }
    }

    /* Print bitrate into info.txt */

    if (infoOutput) {
        infoOutput << "Bitrate: " << endl;

        if (multInfo.delivery) {
            vector<BitratePerPID> bitrates;

            // calculate bitarates
            for (const pair<uint16_t, shared_ptr<PacketStream> >& keyVal: streamsMap) {
                bitrates.push_back(keyVal.second->calculateBitRate(multInfo.delivery->bandwidth, multInfo.delivery->codeRate, multInfo.delivery->constellation, multInfo.delivery->guardinterval));
            }

            // sort bitrates by their speed
            sort(bitrates.begin(), bitrates.end(), greater<BitratePerPID>());

            // write bitarates into info.txt
            for (const BitratePerPID bitRatePerPID : bitrates) {
                infoOutput << "0x" << hex << setfill('0') << setw(4) << bitRatePerPID.PID << " ";
                infoOutput << setprecision(2) << fixed << bitRatePerPID.bitrate << " Mbps" << endl;
            }
        }
    }

    /* Close all output files */

    // Close info.txt output
    infoOutput.close();

    // Close stream outputs
    for (const pair<uint16_t, shared_ptr<PacketStream> >& keyVal: streamsMap) {
        keyVal.second->close();
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    /* Check that is passed one argument  */
    if (argc < 2) {
        cerr << "Missing argument that specifies path to the file with MPEG-2 stream!" << endl;
        return EXIT_FAILURE;
    } else if (argc > 2) {
        cerr << "Too many program arguments! Expected only one argument!" << endl;
    }

    /* Parse filename with MPEG2 transport stream */
    string filename(argv[1]);
    if (filename.size() < 3 || filename.substr(filename.size() - 3) != string(".ts")) {
        cerr << "Input transport stream filename should have an extension .ts!" << endl;
        return EXIT_FAILURE;
    }
    if (filename.find('/') != string::npos) {
        cerr << "Input transport stream filename should not contain '/'!" << endl;
        return EXIT_FAILURE;
    }
    filename = filename.substr(0, filename.size() - 3);

    /* Open input MPEG-2 stream */
    MPEG2FileInputStream is;
    is.open(argv[1], ios::in | ifstream::binary );

    if( !is ) {
        cerr << "Failed to open file!" << endl;
        return EXIT_FAILURE;
    }

    /* Read program specifiec tables */
    PSITables tables;
    if (readPSITables(is, tables) != EXIT_SUCCESS) {
        cerr << "Unable to read some neccessary service information tables!" << endl;
        is.close();
        return EXIT_FAILURE;
    }

    /* Get info to parsed tables */
    MultiplexInfo multiplexInfo;
    multiplexInfo.fileName = filename;
    if (getMultiplexInfo(tables, multiplexInfo) != EXIT_SUCCESS) {
        cerr << "Unable to read informations about multiplex and services from information tables!" << endl;
        is.close();
        return EXIT_FAILURE;
    }

    /* Save multiplex info */
    if (saveMultiplexInfo(is, multiplexInfo) != EXIT_SUCCESS) {
        cerr << "Unable to save informations about multiplex!" << endl;
        is.close();
        return EXIT_FAILURE;
    }

    is.close();

    return EXIT_SUCCESS;
}
