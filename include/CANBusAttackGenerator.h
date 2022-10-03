//
// Created by Tompnyx on 01/09/2022.
//

#ifndef CANBusAttackGenerator_h
#define CANBusAttackGenerator_h

#include <Arduino.h>

/**
 * Check to see if the CAN message was sent successfully or not.
 *
 * @param report The report generated when sending the CAN message
 * @return If the message was sent successfully or not
 */
bool check_message_sent(int report);

/**
 * Performs an impersonation attack
 */
[[noreturn]] void impersonation();

/**
 * Performs a fuzzing attack
 */
void fuzzing();

/**
 * Performs a replay attack
 */
[[noreturn]] void replay();

/**
 * Performs a Denial-of-Service (DoS) attack
 */
void dos();

/**
 * Performs a Frame-drop attack
 */
void frameDrop();

/**
 * Performs sniffing (Package reconnaissance)
 */
[[noreturn]] void sniffing();

/**
 * Generates a random CAN ID.
 *
 * @param id The CAN ID variable to change
 * @param ext If the CAN message is using an extended frame
 */
void generate_random_id(unsigned long &id, bool ext);

/**
 * Generates a random payload of a random length.
 *
 * @param len The length of the payload variable to change
 * @param payload The payload variable to change
 * @param ext If the CAN message is using an extended frame
 * @param rtr If the CAN message is a remote request frame
 */
void generate_random_payload(byte &len, unsigned char *payload, bool ext,
                             bool rtr);

/**
 * Generates a random CAN message.
 *
 * @param id The CAN ID variable to change
 * @param ext If the CAN message is using an extended frame
 * @param rtr If the CAN message is a remote request frame
 * @param len The length of the payload variable to change
 * @param payload The payload variable to change
 */
void generate_random_message(unsigned long &id,
                             byte &ext,
                             byte &rtr,
                             byte &len,
                             unsigned char *payload);

/**
 * Prints a CAN message to the SERIAL monitor.
 *
 * @param canId The CAN ID to print
 * @param len The length of the CAN payload
 * @param buf The CAN payload to print
 */
void print_can_message_to_monitor(unsigned long canId,
                                  byte len,
                                  unsigned char *buf);

/**
 * Receive a CAN message and print it to the SERIAL terminal.
 */
void receive_can();

/**
 * Sends a random or empty CAN message.
 *
 * @param sendRandom True if a random message should be sent, False if an
 * empty message should be sent.
 */
void send_can(bool sendRandom);

/**
 * Sends a CAN message with a randomly generated payload.
 *
 * @param id The ID of the message to send
 * @param ext_condition If the CAN message is using an extended frame
 * @param rtr_condition If the CAN message is a remote request frame
 */
void send_can(unsigned long id, bool ext_condition, bool rtr_condition);

/**
 * Sends a CAN message.
 *
 * @param id The ID of the message to send
 * @param ext_condition If the CAN message is using an extended frame
 * @param rtr_condition If the CAN message is a remote request frame
 * @param payload The payload of the message to send
 * @param len The length of the payload
 */
void send_can(unsigned long id, byte ext, byte rtr, byte len,
              unsigned char *payload);
#endif
