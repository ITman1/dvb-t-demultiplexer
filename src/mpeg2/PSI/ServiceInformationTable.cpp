/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          ServiceInformationTable.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro přečtení servisní tabulky ze streamu
 *
 ******************************************************************************/

/**
 * @file ServiceInformationTable.cpp
 *
 * @brief Module for reading servise information table.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <algorithm>
#include <stdexcept>
#include <memory>

#include <iterator>
#include <vector>

#include "ServiceInformationTable.h"

/**
 * Reads service information table from the stream
 * @param stream Transport stream
 * @param trackPID PID of the table
 * @return Service information table on success, otherwise null
 */
shared_ptr<ServiceInformationTable> ServiceInformationTable::fromPacketStream(MPEG2InputStream &stream, uint16_t trackPID) {
    shared_ptr<ServiceInformationTable> sit(new ServiceInformationTable());

    nonrecursive_reset:

    vector<uint8_t> sit_data;

    unsigned int bytesToRead = PSI_MINSIZE;
    bool headerLoaded = false;

    sit->pid = trackPID;

    unsigned int tableSize = PSI_HEADER_SIZE;
    uint16_t tablePackets = 0;
    int previousContinuityCounter = -1;
    MPEG2InputStream::iterator &packetsIter = stream.current();
    for (; packetsIter != stream.end(); ++packetsIter) {
        const MPEG2Packet &packet = *packetsIter;

        // Reset reading of current table when packet probably belongs to the table and is corrupted
        if (packet.header->PID == trackPID && packet.header->transportErrorIndicator) {
            packetsIter++;
            goto nonrecursive_reset;
        }

        // Continue if some packet is corrupted
        if (packet.header->transportErrorIndicator) {
            continue;
        }

        // Continue if it is not demanded packet
        if (packet.header->PID != trackPID) {
            continue;
        }

        // Reset reading of current table when packet belongs to the table and continuity counter is not correct
        int newContinuityCounter = (previousContinuityCounter + 1) % MPEG2Header::CONTINUITY_COUTER_SIZE;
        newContinuityCounter = (previousContinuityCounter == -1)? packet.header->continuityCounter : newContinuityCounter;
        if (packet.header->continuityCounter != newContinuityCounter) {
            packetsIter++;
            goto nonrecursive_reset;
        }
        previousContinuityCounter = newContinuityCounter;

        // Continue if packet does not transfer anything useful for us
        if (!packet.header || !packet.payload || packet.payload->data.empty()) {
            continue;
        }

        vector<uint8_t> &pData = packet.payload->data;

        /* First packet of the table */
        if (packet.header->payloadUnitStartIndicator) {
            uint8_t pointerField = pData[0];

            // Test for length malformation
            if ((unsigned)(pointerField + 1) >= pData.size()) {
                throw runtime_error ("Pointer field points after the packet end!");
            }

            if (sit_data.empty()) { // first packet, insert all
                sit_data.insert(sit_data.end(), &pData[pointerField + 1], &pData[pData.size()]);
            } else { // probably the last packet, insert the rest
                if (bytesToRead > pointerField) {
                    throw runtime_error ("Another SIT starts, but previous has not been fully loaded yet!");
                }
                sit_data.insert(sit_data.end(), &pData[1], &pData[pointerField + 1]);
            }
        }
        /* Body packets of the table, or trash of the previous table */
        else {
            // Continue if we are reading previous incomplete table
            if (sit_data.empty()) {
                continue;
            }

            // Insert body of the table into vector
            if (bytesToRead >= pData.size()) { // insert all payload
                sit_data.insert(sit_data.end(), pData.begin(), pData.end());
            } else { // insert only demanded number of bytes
                sit_data.insert(sit_data.end(), &pData[0], &pData[bytesToRead]);
            }
        }

        // Skip processing the table, it is not enough for reading the header
        if (sit_data.size() < PSI_HEADER_SIZE) {
            continue;
        }

        // Increment counter of the table packets
        if (!sit_data.empty()) {
            tablePackets++;
        }

        // Read header of the SIT
        if (!headerLoaded) {
            headerLoaded = true;
            sit->tableID = sit_data[0];
            sit->sectionSyntaxIndicator = sit_data[1] & 0x80;
            sit->sectionLength = (sit_data[1] & 0x0F) << 8;
            sit->sectionLength |= sit_data[2];
        }

        // Evaluate size of the whole table
        tableSize = sit->sectionLength + PSI_HEADER_SIZE;

        /* We have read also stuffing bytes, so remove them */
        if (sit_data.size() > tableSize) {
            sit_data.resize(tableSize);
        }

        // Evaluate number of bytes which are leaving
        bytesToRead = tableSize - sit_data.size();
        if (bytesToRead <= 0) {
            bytesToRead = 0;
            break;
        }
    }

    // Table was inside only one packet, so increament iterator
    // => avoiding infinity loop
    if (tablePackets == 1 && packetsIter != stream.end()) {
        packetsIter++;
    }

    // if sizes are correct, fill the section part of the SIT
    if (sit_data.size() == tableSize && tableSize > PSI_HEADER_SIZE) {
        sit_data.erase(sit_data.begin(), sit_data.begin() + PSI_HEADER_SIZE);
        sit->section = sit_data;
    } else {
        // throw runtime_error ("SIT unsucessfully loaded!");
        return shared_ptr<ServiceInformationTable>(0);
    }

    return sit;
}
