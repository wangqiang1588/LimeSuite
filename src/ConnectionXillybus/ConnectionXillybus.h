/**
    @file ConnectionSTREAM.h
    @author Lime Microsystems
    @brief Implementation of STREAM board connection.
*/

#pragma once
#include <ConnectionRegistry.h>
#include <ILimeSDRStreaming.h>
#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <thread>
#include "fifo.h"

#ifndef __unix__
#include "windows.h"
#else
#include <mutex>
#include <condition_variable>
#include <chrono>
#endif

namespace lime{

class ConnectionXillybus : public ILimeSDRStreaming
{
public:
    ConnectionXillybus(const unsigned index);
    ~ConnectionXillybus(void);

    int Open(const unsigned index);
    void Close();
    bool IsOpen();
    int GetOpenedIndex();

    int Write(const unsigned char *buffer, int length, int timeout_ms = 100) override;
    int Read(unsigned char *buffer, int length, int timeout_ms = 100) override;

    //hooks to update FPGA plls when baseband interface data rate is changed
    int UpdateExternalDataRate(const size_t channel, const double txRate, const double rxRate) override;
    int UpdateExternalDataRate(const size_t channel, const double txRate, const double rxRate, const double txPhase, const double rxPhase)override;
    int ReadRawStreamData(char* buffer, unsigned length, int epIndex, int timeout_ms = 100)override;
    int TransferPacket(GenericPacket &pkt) override;
protected:
    void ReceivePacketsLoop(Streamer* args) override;
    void TransmitPacketsLoop(Streamer* args) override;

    int ReceiveData(char* buffer, int length, int epIndex, int timeout = 100) override;
    int SendData(const char* buffer, int length, int epIndex, int timeout = 100) override;
    void AbortReading(int epIndex);
    void AbortSending(int epIndex);

private:
    static const int MAX_EP_CNT = 2;
    struct EPConfig
    {
        std::string ctrlRead;
        std::string ctrlWrite;
        std::string streamRead[MAX_EP_CNT];
        std::string streamWrite[MAX_EP_CNT];
    };

    static const EPConfig deviceConfigs[];
    eConnectionType GetType(void)
    {
        return PCIE_PORT;
    }

    std::string m_hardwareName;
    int m_hardwareVer;

    bool isConnected;
    std::mutex mTransferLock;
#ifndef __unix__
    HANDLE hWrite;
    HANDLE hRead;
    HANDLE hWriteStream[MAX_EP_CNT];
    HANDLE hReadStream[MAX_EP_CNT];
#else
    int hWrite;
    int hRead;
    int hWriteStream[MAX_EP_CNT];
    int hReadStream[MAX_EP_CNT];
#endif
    std::string writeCtrlPort;
    std::string readCtrlPort;
    std::string writeStreamPort[MAX_EP_CNT];
    std::string readStreamPort[MAX_EP_CNT];
};



class ConnectionXillybusEntry : public ConnectionRegistryEntry
{
public:
    ConnectionXillybusEntry(void);

    ~ConnectionXillybusEntry(void);

    std::vector<ConnectionHandle> enumerate(const ConnectionHandle &hint);

    IConnection *make(const ConnectionHandle &handle);

private:
    #ifndef __unix__
    std::string DeviceName(unsigned int index);
    #else
    #endif
};

}
