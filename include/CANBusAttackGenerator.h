//
// Created by tompnyx on 01/09/2022.
//

#ifndef CANBusAttackGenerator_h
#define CANBusAttackGenerator_h

#include <Arduino.h>

// Performs an impersonation attack
void impersonation();
// Performs a fuzzing attack
void fuzzing();
// Performs a replay attack
void replay();
// Performs a Denial-of-Service (DoS) attack
void dos();
// Performs a Frame-drop attack
void frameDrop();

void generate_random_id(unsigned long &id, bool ext);
void generate_random_payload(byte &len, unsigned char *payload, bool ext,
                             bool rtr);
void generate_random_message(unsigned long &id,
                             byte &ext,
                             byte &rtr,
                             byte &len,
                             unsigned char *payload);
void print_can_message_to_monitor(unsigned long canId,
                                  byte len,
                                  unsigned char *buf);
bool check_message_sent(int report);
void send_can(bool sendRandom);
void send_can(unsigned long id, bool ext_condition, bool rtr_condition);
void send_can(unsigned long id, byte ext, byte rtr, byte len,
              unsigned char *payload);
#endif
