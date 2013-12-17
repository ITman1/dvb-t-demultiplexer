/*******************************************************************************
 * Projekt:         Projekt č.2: Demultiplexing transportního streamu DVB-T
 * Předmět:         Bezdrátové a mobilní sítě
 * Soubor:          MPEG2AudioFileStream.cpp
 * Datum:           Prosinec 2013
 * Jméno:           Radim
 * Příjmení:        Loskot
 * Login autora:    xlosko01
 * E-mail:          xlosko01(at)stud.fit.vutbr.cz
 * Popis:           Mudul pro definující třídu výstupního audio streamu do soubor.
 *
 ******************************************************************************/

/**
 * @file MPEG2AudioFileStream.cpp
 *
 * @brief Module which defines class of the output stream into the file.
 * @author Radim Loskot xlosko01(at)stud.fit.vutbr.cz
 */

#include <algorithm>

#include <bass.h>

#include "MPEG2AudioFileStream.h"

/**
 * Initialization variable
 */
bool MPEG2AudioFileStream::bass_initialized = false;

#ifdef _BIG_ENDIAN
inline DWORD le_32(DWORD v)
{
    return (v>>24)|((v>>8)&0xff00)|((v&0xff00)<<8)|(v<<24);
}
inline WORD le_16(WORD v)
{
    return (v>>8)|(v<<8);
}
#else
#define le_32(v) (v)
#define le_16(v) (v)
#endif

/**
 * Writes vector of data into .wav file
 *
 * @param filename Name of the file where to put audio data
 * @param data Audio data vector.
 */
void MPEG2AudioFileStream::writeWaw(string filename, vector<uint8_t> &data) {
    BASS_CHANNELINFO info;
    DWORD p;
    FILE *fp;
    short buf[10000];
    WAVEFORMATEX wf;

    bool bass_wasInitialized = bass_initialized;
    bass_initialized = (!bass_initialized)? BASS_Init(-1 ,44100, 0, 0, NULL) : true;

    /* Open stream for reading from the buffer. */
    HSTREAM chan = BASS_StreamCreateFile(TRUE, &data[0], 0, data.size(), BASS_STREAM_DECODE);

    if (chan == 0) {
        goto writeWawReturn;
    }

    /* Open file for writing audio data. */

    if (!(fp = fopen(filename.c_str(), "wb"))) {
        goto writeWawReturn;
    }

    /* Write WAV header */
    BASS_ChannelGetInfo(chan,&info);

    wf.wFormatTag = 1;
    wf.nChannels = info.chans;
    wf.wBitsPerSample = (info.flags&BASS_SAMPLE_8BITS?8:16);
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nSamplesPerSec = info.freq;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

#ifdef _BIG_ENDIAN // swap byte order
    wf.wFormatTag = le_16(wf.wFormatTag);
    wf.nChannels = le_16(wf.nChannels);
    wf.wBitsPerSample = le_16(wf.wBitsPerSample);
    wf.nBlockAlign = le_16(wf.nBlockAlign);
    wf.nSamplesPerSec = le_32(wf.nSamplesPerSec);
    wf.nAvgBytesPerSec = le_32(wf.nAvgBytesPerSec);
#endif
    fwrite("RIFF\0\0\0\0WAVEfmt \20\0\0\0", 20, 1, fp);
    fwrite(&wf, 16, 1, fp);
    fwrite("data\0\0\0\0", 8, 1, fp);

    /* Write .wav audio data */

    while (BASS_ChannelIsActive(chan)) {
        int c = BASS_ChannelGetData(chan, buf, 20000);
#ifdef _BIG_ENDIAN
        if (!(info.flags&BASS_SAMPLE_8BITS)) // swap 16-bit byte order
            for (p=0; p < c / 2; p++) buf[p] = le_16(buf[p]);
#endif
        fwrite(buf,1,c,fp);
    }

    /* Complete WAV header */
    fflush(fp);
    p = ftell(fp);
    fseek(fp, 4, SEEK_SET);
    putw(le_32(p-8), fp);
    fflush(fp);
    fseek(fp, 40, SEEK_SET);
    putw(le_32(p - 44), fp);
    fflush(fp);
    fclose(fp);

    BASS_StreamFree(chan);

writeWawReturn:
    if (!bass_wasInitialized && bass_initialized) {
        BASS_Free();
        bass_initialized = false;
    }
}

/**
 * Callback method which is called if new chunk of audio is available
 * @param packetStream Audio elementary stream
 */
void MPEG2AudioFileStream::onPacketRecieved(const PacketElementaryStream &) {
}

/**
 * Callback method that is called by service base class - delivers recieved fragment.
 * Appending of the data into file is done here.
 * @param streamFragment PES fragment.
 */
void MPEG2AudioFileStream::onFragmentRecieved(const PacketElementaryStreamFragment &streamFragment) {
    /*if (!audioHaderFound) {
        auto it = search(streamFragment.streamData.begin(), streamFragment.streamData.end(), PacketElementaryStream::PES_DATA_HEADER_PREFIX.begin(), PacketElementaryStream::PES_DATA_HEADER_PREFIX.end());
        auto itCopy = it;

        audioHaderFound = itCopy != streamFragment.streamData.end();
        audioHaderFound = (audioHaderFound)? itCopy + PacketElementaryStream::PES_DATA_HEADER_PREFIX.size() + 1 != streamFragment.streamData.end() : false;

        if (audioHaderFound) {
            vector<uint8_t> data(it, streamFragment.streamData.end());
            write(data);
        }
    } else {
        write(streamFragment.streamData);
    }*/
    write(streamFragment.streamData);
}

/**
 * Writes audio data to file.
 * @param streamData Data to be written into the file
 */
void MPEG2AudioFileStream::write(const vector<uint8_t> &streamData) {
    if (!streamData.empty()) {
        data.resize(data.size() + streamData.size());
        copy(streamData.begin(), streamData.end(), &data[data.size() - streamData.size()]);
    }
}

/**
 * Constructs audio file output stream
 * @param PID PID which identifies the service stream of the audio
 */
MPEG2AudioFileStream::MPEG2AudioFileStream(uint16_t PID) : MPEG2ServiceStream(PID), audioHaderFound(false) {

}

/**
 * Opens audio output stream
 * @param filename Name of the file where to stream audio
 */
void MPEG2AudioFileStream::open(string &filename) {
    this->filename = filename;
}

/**
 * Closes audio stream.
 */
void MPEG2AudioFileStream::close() {
    if (!data.empty()) {
        writeWaw(filename, data);
    }
}

/**
 * Tests state of the audio stream.
 * @return True if stream is sucessufuly opened, otherwise false.
 */
bool MPEG2AudioFileStream::operator!(void) const {
    return true;
}
