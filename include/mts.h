/*
Copyright (C) 2016  Davidson Francis <davidsondfgl@gmail.com>
              2016  Ádamo Ludwig     <Ludwigadamo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef MTS_H_
#define MTS_H_

/* Some constants. */
#define PORT 1234
#define MAX_CLIENTS 4
#define MSG_SIZE 16

/* Messages types. */
#define MSG_HELLO        0xBEEF
#define MSG_READY        0xACCE55
#define MSG_TIMEREQUEST  0xDECADE
#define MSG_TIMERESPONSE 0x5EED

/* 
 * Calcules the Round Trip Time (in seconds),
 * to adjust the delays in communication.
 */
#define RTT(n) (((n).data.b) - ((n).data.a)) \
             - (((n).data.y) - ((n).data.x))

/* Message type. */
struct message
{
    uint32_t type;
    union
    {
        /* For RTT calculation. */
        struct
        {
            uint64_t x;
            uint64_t y;
            uint64_t a;
            uint64_t b;
        };

        /* An arbitrary message. */
        uint8_t msg[MSG_SIZE];
    }data;
} __attribute__((packed));

#endif /* MTS_H_ */
