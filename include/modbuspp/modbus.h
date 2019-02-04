//
// Created by Fanzhe on 5/28/2017.
//

#ifndef MODBUSPP_MODBUS_H
#define MODBUSPP_MODBUS_H

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h> 
#include "modbus_exception.h"

using namespace std;

#define MAX_MSG_LENGTH 260

///Function Code
enum{
    READ_COILS       = 0x01,
    READ_INPUT_BITS  = 0x02,
    READ_REGS        = 0x03,
    READ_INPUT_REGS  = 0x04,
    WRITE_COIL       = 0x05,
    WRITE_REG        = 0x06,
    WRITE_COILS      = 0x0F,
    WRITE_REGS       = 0x10,
};

///Exception Codes
enum {
    EX_ILLEGAL_FUNCTION = 0x01, // Function Code not Supported
    EX_ILLEGAL_ADDRESS  = 0x02, // Output Address not exists
    EX_ILLEGAL_VALUE    = 0x03, // Output Value not in Range
    EX_SERVER_FAILURE   = 0x04, // Slave Deive Fails to process request
    EX_ACKNOWLEDGE      = 0x05, // Service Need Long Time to Execute
    EX_SERVER_BUSY      = 0x06, // Server Was Unable to Accept MB Request PDU
    EX_GATEWAY_PROBLEMP = 0x0A, // Gateway Path not Available
    EX_GATEWYA_PROBLEMF = 0x0B, // Target Device Failed to Response
};


/// Modbus Operator Class
/**
 * Modbus Operator Class
 * Providing networking support and mobus operation support.
 */
namespace modbuspp
{

class Modbus {
private:
    bool connected_;
    uint16_t PORT;
    int socket_;
    int msg_id_;
    int slaveid_ ;
    string HOST;
    struct sockaddr_in server_;
    fd_set socket_fd_;
    long block_arg_;
    struct timeval tv_;
    socklen_t lon_;
    int valopt_;
    int res_;
    
    void modbusBuildRequest(uint8_t *to_send,int address, int func);

    void modbusRead(int address, int amount, int func);
    void modbusWrite(int address, int amount, int func, uint16_t *value);

    ssize_t modbusSend(uint8_t *to_send, int length);
    ssize_t modbusReceive(uint8_t *buffer);

    void modbusErrorHandle(uint8_t *msg, int func);


public:
    Modbus(string host, uint16_t port);
    Modbus(string host);
    ~Modbus();

    bool modbusConnect(int = 5);
    void modbusClose();

    void modbusSetSlaveId(int id);

    void modbusReadCoils(int address, int amount, bool* buffer);
    void modbusReadInputBits(int address, int amount, bool* buffer);
    void modbusReadHoldingRegisters(int address, int amount, uint16_t *buffer);
    void modbusReadInputRegisters(int address, int amount, uint16_t *buffer);

    void modbusWriteCoil(int address, bool to_write);
    void modbusWriteRegister(int address, uint16_t value);
    void modbusWriteCoils(int address, int amount, bool* value );
    void modbusWriteRegisters(int address, int amount, uint16_t *value);
};

}; // namespace modbuspp
#endif //MODBUSPP_MODBUS_H
