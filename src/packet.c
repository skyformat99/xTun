#include <string.h>
#include <assert.h>

#include "util.h"
#include "packet.h"


int
packet_filter(struct packet *packet, const char *buf, ssize_t buflen) {
    int rc;

    if (packet->size == 0) {
        assert(buflen <= HEADER_BYTES);

        if (packet->read == 1) {
            packet->size = read_size((uint8_t *) packet->buf);
            if (packet->size <= packet->max) {
                rc = PACKET_UNCOMPLETE;
            } else {
                rc = PACKET_INVALID;
            }

        } else {
            if (buflen == 1) {
                packet->read = 1;
                rc = PACKET_UNCOMPLETE;

            } else if (buflen == HEADER_BYTES) {
                packet->size = read_size((uint8_t *) buf);
                if (packet->size > 0 && packet->size <= packet->max) {
                    rc = PACKET_UNCOMPLETE;
                } else {
                    rc = PACKET_INVALID;
                }

            } else {
                rc = PACKET_INVALID;
            }
        }

    } else {
        if (buflen + packet->offset < packet->size) {
            packet->offset += buflen;
            rc = PACKET_UNCOMPLETE;

        } else {
            assert(buflen + packet->offset == packet->size);
            rc = PACKET_COMPLETED;
        }
    }

    return rc;
}

void
packet_reset(struct packet *packet) {
    packet->read = 0;
    packet->offset = 0;
    packet->size = 0;
}
