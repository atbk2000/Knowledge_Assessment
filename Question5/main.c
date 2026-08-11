#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ============================================================================
// PROTOCOL DEFINITIONS & CONFIGURATIONS
// ============================================================================
#define START_BYTE    0xAA
#define MAX_PAYLOAD   64
#define NUM_NODES     4

// Structure representing a complete UART frame
typedef struct {
    uint8_t start_byte;
    uint8_t src_id;
    uint8_t dest_id;
    uint8_t command;
    uint8_t length;
    int8_t payload[MAX_PAYLOAD];
    uint8_t checksum;
} UART_Frame;

// ============================================================================
// HARDWARE ABSTRACTION LAYER & INTERRUPT SIMULATION
// ============================================================================

// Ring buffer simulating the shared UART physical line
typedef struct {
    uint8_t buffer[256];
    uint16_t head;
    uint16_t tail;
} UART_RingBuffer;

static UART_RingBuffer rx_fifo = { .head = 0, .tail = 0 };

/**
 * @brief INTERRUPT HANDLER (ISR)
 * Triggered automatically by hardware whenever 1 new byte arrives on UART RX.
 * @param hardware_byte The byte received from the UART hardware data register.
 */
void UART_RX_IRQHandler(uint8_t hardware_byte) {
    // Store byte directly into the RX buffer
    rx_fifo.buffer[rx_fifo.head] = hardware_byte;
    rx_fifo.head = (rx_fifo.head + 1) % 256;
}

/**
 * @brief Low-level helper to read a byte from the RX buffer populated by ISR.
 * @param byte Pointer where the read byte will be saved.
 * @return true if byte was read successfully, false if the buffer is empty.
 */
bool uart_lowlevel_read_byte(uint8_t *byte) {
    if (rx_fifo.head == rx_fifo.tail) {
        return false; // No data available
    }
    *byte = rx_fifo.buffer[rx_fifo.tail];
    rx_fifo.tail = (rx_fifo.tail + 1) % 256;
    return true;
}

// ============================================================================
// CHECKSUM & PARSER FUNCTIONS
// ============================================================================

/**
 * @brief Calculates the XOR Checksum of a byte array.
 * @param data Pointer to the buffer.
 * @param len Number of bytes to calculate.
 * @return uint8_t Computed checksum value.
 */
uint8_t calculate_checksum(const uint8_t *data, uint8_t len) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * @brief Constructs and sends a command packet with signed values.
 * 
 * @param src_id Sender Node ID (1-4)
 * @param dest_id Receiver Node ID (1-4)
 * @param message String message to transmit
 */
void node_send_command(uint8_t src_id, uint8_t dest_id, uint8_t command, const int8_t* signed_data, uint8_t len) {
    UART_Frame frame;
    frame.start_byte = START_BYTE;
    frame.src_id = src_id;
    frame.dest_id = dest_id;

    if (len > MAX_PAYLOAD){
        frame.length = MAX_PAYLOAD;
    }
    else{
        frame.length = len;
    }

    memcpy(frame.payload, signed_data, frame.length);

    // Compute checksum over header fields + signed payload
    uint8_t header[4] = { frame.src_id, frame.dest_id, frame.command, frame.length };
    uint8_t header_chk = calculate_checksum(header, 4);
    uint8_t payload_chk = calculate_checksum((const uint8_t*)frame.payload, frame.length);
    frame.checksum = header_chk ^ payload_chk;

    // Simulate physical transmission: trigger RX interrupt byte-by-byte on the bus
    UART_RX_IRQHandler(frame.start_byte);
    UART_RX_IRQHandler(frame.src_id);
    UART_RX_IRQHandler(frame.dest_id);
    UART_RX_IRQHandler(frame.command);
    UART_RX_IRQHandler(frame.length);

    for (uint8_t i = 0; i < frame.length; i++) {
        UART_RX_IRQHandler((uint8_t)frame.payload[i]);
    }

    UART_RX_IRQHandler(frame.checksum);

    printf("[Node %d] Sent CMD 0x%02X to Node %d with %d signed values.\n", src_id, command, dest_id, frame.length);
}

/**
 * @brief PARSER FUNCTION
 * @param current_node_id ID of the receiving node trying to process data.
 */
void node_parse_rx_buffer(uint8_t current_node_id) {
    uint8_t byte;

    while (uart_lowlevel_read_byte(&byte)) {
        if (byte == START_BYTE) {
            UART_Frame frame;
            frame.start_byte = byte;

            // Read header 
            if (!uart_lowlevel_read_byte(&frame.src_id)) return;
            if (!uart_lowlevel_read_byte(&frame.dest_id)) return;
            if (!uart_lowlevel_read_byte(&frame.command)) return;
            if (!uart_lowlevel_read_byte(&frame.length)) return;

            // Read signed payload
            for (uint8_t i = 0; i < frame.length; i++) {
                uint8_t raw_byte;
                if (!uart_lowlevel_read_byte(&raw_byte)) return;
                frame.payload[i] = (int8_t)raw_byte;
            }

            // Read checksum
            if (!uart_lowlevel_read_byte(&frame.checksum)) return;

            // Step 1: Address Check
            if (frame.dest_id != current_node_id) {
                printf("[Node %d] Ignored packet (Intended for Node %d).\n", current_node_id, frame.dest_id);
                return;
            }

            // Step 2: Checksum integrity check
            uint8_t header[4] = { frame.src_id, frame.dest_id, frame.command, frame.length };
            uint8_t expected_chk = calculate_checksum(header, 4) ^ calculate_checksum((const uint8_t*)frame.payload, frame.length);

            if (frame.checksum != expected_chk) {
                printf("[Node %d] ERROR: Checksum mismatch! Corrupted packet.\n", current_node_id);
                return;
            }

            // 3. Execution / Command Handler
            printf("[Node %d] SUCCESS: Parsed Command 0x%02X from Node %d!\n", current_node_id, frame.command, frame.src_id);
            printf("           Payload Values: ");
            for (uint8_t i = 0; i < frame.length; i++) {
                printf("%d ", frame.payload[i]); // Prints negative and positive values correctly
            }
            printf("\n");
            return;
        }
    }
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    printf("=== INTERRUPT-DRIVEN UART PARSER DEMO ===\n\n");

    // Command 0x20 with both positive and negative values (e.g., temperature adjustments)
    int8_t sensor_readings[] = {-25, 10, -5, 42, 0};
    uint8_t data_count = sizeof(sensor_readings) / sizeof(sensor_readings[0]);

    // Transmit from Node 1 to Node 3
    node_send_command(1, 3, 0x20, sensor_readings, data_count);

    printf("\n------------------------------------------------\n\n");

    // Node 3 parses the incoming ISR buffer
    node_parse_rx_buffer(3);

    return 0;
}