//
// Created by Fanzhe on 5/28/2017.
//

#include "modbuspp/modbus.h"
#include <arpa/inet.h>
using namespace std;
using namespace modbuspp;

/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
Modbus::Modbus(string host, uint16_t port) {
    HOST = host;
    PORT = port;
    slaveid_ = 1;
    msg_id_ = 1;
    connected_ = false;

}


/**
 * Overloading Modbus Connector Constructor with Default Port Set at 502
 * @param host  IP Address of Host
 * @return      A Modbus Connector Object
 */
Modbus::Modbus(string host) {
    Modbus(host, 502);
}


/**
 * Destructor of Modbus Connector Object
 */
Modbus::~Modbus(void) {
}


/**
 * Modbus Slave ID Setter
 * @param id  ID of the Modbus Server Slave
 */
void Modbus::modbusSetSlaveId(int id) {
    slaveid_ = id;
}


/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
bool Modbus::modbusConnect() {
    if(HOST == "" || PORT == 0) {
        std::cout << "MODBUS: Missing Host and Port" << std::endl;
        return false;
    } else {
        std::cout << "MODBUS: Found Proper Host "<< HOST << " and Port " <<PORT <<std::endl;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_ == -1) {
        std::cout <<"MODBUS: Error Opening Socket" <<std::endl;
        return false;
    } else {
        std::cout <<"MODBUS: Socket Opened Successfully" << std::endl;
    }

    server_.sin_family = AF_INET;
    server_.sin_addr.s_addr = inet_addr(HOST.c_str());
    server_.sin_port = htons(PORT);

    if (connect(socket_, (struct sockaddr*)&server_, sizeof(server_)) < 0) {
        std::cout<< "MODBUS: Connection Error" << std::endl;
        return false;
    }

    std::cout<< "MODBUS: Connected" <<std::endl;
    connected_ = true;
    return true;
}


/**
 * Close the Modbus/TCP Connection
 */
void Modbus::modbusClose() {
    close(socket_);
    std::cout <<"MODBUS: Socket Closed" <<std::endl;
}


/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
void Modbus::modbusBuildRequest(uint8_t *to_send, int address, int func) {
    to_send[0] = (uint8_t) msg_id_ >> 8;
    to_send[1] = (uint8_t) (msg_id_ & 0x00FF);
    to_send[2] = 0;
    to_send[3] = 0;
    to_send[4] = 0;
    to_send[6] = (uint8_t) slaveid_;
    to_send[7] = (uint8_t) func;
    to_send[8] = (uint8_t) (address >> 8);
    to_send[9] = (uint8_t) (address & 0x00FF);
}


/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
void Modbus::modbusWrite(int address, int amount, int func, uint16_t *value) {
    if(func == WRITE_COIL || func == WRITE_REG) {
        uint8_t to_send[12];
        modbusBuildRequest(to_send, address, func);
        to_send[5] = 6;
        to_send[10] = (uint8_t) (value[0] >> 8);
        to_send[11] = (uint8_t) (value[0] & 0x00FF);
        modbusSend(to_send, 12);
    } else if(func == WRITE_REGS){
        uint8_t to_send[13 + 2 * amount];
        modbusBuildRequest(to_send, address, func);
        to_send[5] = (uint8_t) (5 + 2 * amount);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) (amount & 0x00FF);
        to_send[12] = (uint8_t) (2 * amount);
        for(int i = 0; i < amount; i++) {
            to_send[13 + 2 * i] = (uint8_t) (value[i] >> 8);
            to_send[14 + 2 * i] = (uint8_t) (value[i] & 0x00FF);
        }
        modbusSend(to_send, 13 + 2 * amount);
    } else if(func == WRITE_COILS) {
        uint8_t to_send[14 + (amount -1) / 8 ];
        modbusBuildRequest(to_send, address, func);
        to_send[5] = (uint8_t) (7 + (amount -1) / 8);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) (amount >> 8);
        to_send[12] = (uint8_t) ((amount + 7) / 8);
        for(int i = 0; i < amount; i++) {
            to_send[13 + (i - 1) / 8] += (uint8_t) (value[i] << (i % 8));
        }
        modbusSend(to_send, 14 + (amount - 1) / 8);
    }
}


/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
void Modbus::modbusRead(int address, int amount, int func){
    uint8_t to_send[12];
    modbusBuildRequest(to_send, address, func);
    to_send[5] = 6;
    to_send[10] = (uint8_t) (amount >> 8);
    to_send[11] = (uint8_t) (amount & 0x00FF);
    modbusSend(to_send, 12);
}


/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
void Modbus::modbusReadHoldingRegisters(int address, int amount, uint16_t *buffer) {
    if(connected_) {
        if(amount > 65535 || address > 65535) {
            throw ModbusAmountException();
        }
        modbusRead(address, amount, READ_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try {
            modbusErrorHandle(to_rec, READ_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Read Input Registers 
 * MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 */
void Modbus::modbusReadInputRegisters(int address, int amount, uint16_t *buffer) {
    if(connected_){
        if(amount > 65535 || address > 65535) {
            throw ModbusAmountException();
        }
        modbusRead(address, amount, READ_INPUT_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try {
            modbusErrorHandle(to_rec, READ_INPUT_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Read Coils           
 * MODBUS FUNCTION 0x01
 * @param address     Reference Address
 * @param amount      Amount of Coils to Read
 * @param buffer      Buffer to Store Data Read from Coils
 */
void Modbus::modbusReadCoils(int address, int amount, bool *buffer) {
    if(connected_) {
        if(amount > 2040 || address > 65535) {
            throw ModbusAmountException();
        }
        modbusRead(address, amount, READ_COILS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try {
            modbusErrorHandle(to_rec, READ_COILS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Read Input Bits(Discrete Data)
 * MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 */
void Modbus::modbusReadInputBits(int address, int amount, bool* buffer) {
    if(connected_) {
        if(amount > 2040 || address > 65535) {
            throw ModbusAmountException();
        }
        modbusRead(address, amount, READ_INPUT_BITS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try {
            modbusErrorHandle(to_rec, READ_INPUT_BITS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
void Modbus::modbusWriteCoil(int address, bool to_write) {
    if(connected_) {
        if(address > 65535) {
            throw ModbusAmountException();
        }
        int value = to_write * 0xFF00;
        modbusWrite(address, 1, WRITE_COIL, (uint16_t *)&value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try{
            modbusErrorHandle(to_rec, WRITE_COIL);
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Write Single Register
 * FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 */
void Modbus::modbusWriteRegister(int address, uint16_t value) {
    if(connected_) {
        if(address > 65535) {
            throw ModbusAmountException();
        }
        modbusWrite(address, 1, WRITE_REG, &value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try{
            modbusErrorHandle(to_rec, WRITE_COIL);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Write Multiple Coils 
 * MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 */
void Modbus::modbusWriteCoils(int address, int amount, bool *value) {
    if(connected_) {
        if(address > 65535 || amount > 65535) {
            throw ModbusAmountException();
        }
        uint16_t temp[amount];
        for(int i = 0; i < 4; i++) {
            temp[i] = (uint16_t)value[i];
        }
        modbusWrite(address, amount, WRITE_COILS,  temp);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try{
            modbusErrorHandle(to_rec, WRITE_COILS);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Write Multiple Registers 
 * MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 */
void Modbus::modbusWriteRegisters(int address, int amount, uint16_t *value) {
    if(connected_) {
        if(address > 65535 || amount > 65535) {
            throw ModbusAmountException();
        }
        modbusWrite(address, amount, WRITE_REGS, value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbusReceive(to_rec);
        try{
            modbusErrorHandle(to_rec, WRITE_REGS);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw ModbusConnectException();
    }
}


/**
 * Data Sender
 * @param to_send Request to Be Sent to Server
 * @param length  Length of the Request
 * @return        Size of the request
 */
ssize_t Modbus::modbusSend(uint8_t *to_send, int length) {
    msg_id_++;
    return send(socket_, to_send, (size_t)length, 0);
}


/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
ssize_t Modbus::modbusReceive(uint8_t *buffer) {
    return recv(socket_, (char *) buffer, 1024, 0);
}


/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
void Modbus::modbusErrorHandle(uint8_t *msg, int func) {
    if(msg[7] == func + 0x80) {
        switch(msg[8]){
            case EX_ILLEGAL_FUNCTION:
                throw ModbusIllegalFunctionException();
            case EX_ILLEGAL_ADDRESS:
                throw ModbusIllegalAddressException();
            case EX_ILLEGAL_VALUE:
                throw ModbusIllegalDataValueException();
            case EX_SERVER_FAILURE:
                throw ModbusServerFailureException();
            case EX_ACKNOWLEDGE:
                throw ModbusAcknowledgeException();
            case EX_SERVER_BUSY:
                throw ModbusServerBusyException();
            case EX_GATEWAY_PROBLEMP:
            case EX_GATEWYA_PROBLEMF:
                throw ModbusGatewayException();
            default:
                break;
        }
    }
}
