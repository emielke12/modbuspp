//
// Created by Fanzhe on 5/29/2017.
//

#ifndef MODBUSPP_MODBUS_EXCEPTION_H
#define MODBUSPP_MODBUS_EXCEPTION_H

#include <exception>
using namespace std;


/// Modbus Exception Class
/**
 * Modbus Exeception Super Class
 *
 * Thrown when a exception or errors happens in modbus protocol
 */
class ModbusException : public exception {
public:
    string msg;
    virtual const char* what() const throw()
    {
        return "Modbus Error.";
    }
};


/// Modbus Connect Exception
/**
 * Connection Issue
 *
 * Thrown when a connection issues happens between modbus client and server
 */
class ModbusConnectException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Connection Problem.";
    }
};


/// Modbus Illgal Function Exception
/**
 * Illegal Function
 *
 * Thrown when modbus server return error response function 0x01
 */
class ModbusIllegalFunctionException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Modbus Function.";
    }
};


/// Modbus Illegal Address Exception
/**
 * Illegal Address
 *
 * Thrown when modbus server return error response function 0x02
 */
class ModbusIllegalAddressException: public ModbusException {
public:
    string msg = "test";
    const char* what() const throw()
    {
        return "Illegal Modbus Address.";
    }
};


/// Modbus Illegal Data Value Exception
/**
 * Illegal Data Vlaue
 *
 * Thrown when modbus server return error response function 0x03
 */
class ModbusIllegalDataValueException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Modbus Data Value.";
    }
};


/// Modbus Server Failure Exception
/**
 * Server Failure
 *
 * Thrown when modbus server return error response function 0x04
 */
class ModbusServerFailureException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Server Failure.";
    }
};


/// Modbus Acknowledge Exception
/**
 * Acknowledge
 *
 * Thrown when modbus server return error response function 0x05
 */
class ModbusAcknowledgeException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Acknowledge Error.";
    }
};


/// Modbus Server Busy Exception
/**
 * Server Busy
 *
 * Thrown when modbus server return error response function 0x06
 */
class ModbusServerBusyException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Server Busy.";
    }
};

/// Modbus Gate Way Problem Exception
/**
 * Gate Way Problem
 *
 * Thrown when modbus server return error response function 0x0A and 0x0B
 */
class ModbusGatewayException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Gateway Problem.";
    }
};

/// Modbus Buffer Exception
/**
 * Buffer Exception
 *
 * Thrown when buffer is too small for the data to be storaged.
 */
class ModbusBufferException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Size of Modbus Buffer Is too Small.";
    }
};


/// Modbus Amount Exception
/**
 * Amount Exception
 *
 * Thrown when the address or amount input is mismatching.
 */
class ModbusAmountException: public ModbusException {
public:
    virtual const char* what() const throw()
    {
        return "Modbus Address too Large.";
    }
};

#endif //MODBUSPP_MODBUS_EXCEPTION_H
