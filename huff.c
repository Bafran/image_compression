#include "huff.h"

#define bitset(byte, nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte, nbit) ((byte) &= ~(1<<(nbit)))

#define BLOCK_SIZE 8

/* This module uses standard huffman tables for both luminosity (Y) and colour components (Cb, Cr) of the image */

HuffmanTable huffman_tables[4] = {
        [LUM_DC_TABLE] = {
                .offsets = { 0,0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0 },
                .symbols = { 0,1,2,3,4,5,6,7,8,9,10,11 },
                .codes = { 0 },
                .set = false
        },
        [LUM_AC_TABLE] = {
                .offsets = { 0,0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,0x7d },
                .symbols = {
                        0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12, /* 0x00: EOB */
                        0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,
                        0x22,0x71,0x14,0x32,0x81,0x91,0xa1,0x08,
                        0x23,0x42,0xb1,0xc1,0x15,0x52,0xd1,0xf0, /* 0xf0: ZRL */
                        0x24,0x33,0x62,0x72,0x82,0x09,0x0a,0x16,
                        0x17,0x18,0x19,0x1a,0x25,0x26,0x27,0x28,
                        0x29,0x2a,0x34,0x35,0x36,0x37,0x38,0x39,
                        0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
                        0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
                        0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
                        0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
                        0x7a,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
                        0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,
                        0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
                        0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,
                        0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,
                        0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,
                        0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,
                        0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,
                        0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,
                        0xf9,0xfa
                },
                .codes = { 0 },
                .set = false
        },
        [COL_DC_TABLE] = {
                .offsets = { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                .symbols = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
                .codes = { 0 },
                .set = false
        },
        [COL_AC_TABLE] = {
                .offsets = { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119 },
                .symbols = {
                        0, 1, 2, 3, 17, 4, 5, 33,
                        49, 6, 18, 65, 81, 7, 97, 113,
                        19, 34, 50, 129, 8, 20, 66, 145,
                        161, 177, 193, 9, 35, 51, 82, 240,
                        21, 98, 114, 209, 10, 22, 36, 52,
                        225, 37, 241, 23, 24, 25, 26, 38,
                        39, 40, 41, 42, 53, 54, 55, 56,
                        57, 58, 67, 68, 69, 70, 71, 72,
                        73, 74, 83, 84, 85, 86, 87, 88,
                        89, 90, 99, 100, 101, 102, 103, 104,
                        105, 106, 115, 116, 117, 118, 119, 120,
                        121, 122, 130, 131, 132, 133, 134, 135,
                        136, 137, 138, 146, 147, 148, 149, 150,
                        151, 152, 153, 154, 162, 163, 164, 165,
                        166, 167, 168, 169, 170, 178, 179, 180,
                        181, 182, 183, 184, 185, 186, 194, 195,
                        196, 197, 198, 199, 200, 201, 202, 210,
                        211, 212, 213, 214, 215, 216, 217, 218,
                        226, 227, 228, 229, 230, 231, 232, 233,
                        234, 242, 243, 244, 245, 246, 247, 248,
                        249, 250
                },
                .codes = { 0 },
                .set = false
        },
};

/* Zig zag orders a matrix into an array, could probably be done with just a list of indexes as a speed improvement */
void zig_zag_order(int *output, float **Matrix){
    int row = 0, col = 0, d = 0;
    int dirs[2][2] = {{-1, 1}, {1, -1}};

    for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++){
        output[i] = (int)Matrix[row][col];

        row += dirs[d][0];
        col += dirs[d][1];

        if (row >= BLOCK_SIZE) { row = BLOCK_SIZE - 1; col += 2; d = 1 - d;}
        if (col >= BLOCK_SIZE) { col = BLOCK_SIZE - 1; row += 2; d = 1 - d;}
        if (row < 0)           { row = 0; d = 1 - d;}
        if (col < 0)           { col = 0; d = 1 - d;}
    }
}

/* This function generates the huffman codes based on the above data showing the lengths of each code and the
 * symbol that's being encoded. Definitely slower than it needs to be, but it's only happening once. Also, the
 * 0x00 and 0xF0 codes are being saved so that they don't have to be fetched in the compression.c file.
*/
void generate_huff_codes(HuffmanTable *table, enum Table_t type){
    /* Calculate the bit rep of all DC or AC symbols */
    // Iterate through the code lengths
    uint16_t bit_rep = 0;
    uint8_t code_num = 0;
    // For every code length
    for (int i = 1; i < 17; i++) {
        // Iterate the number of codes there are of that length
        for (int j = 0; j < table->offsets[i]; j++) {
            // Store the code candidate
            table->codes[code_num] = bit_rep;
            // Add one
            bit_rep++;
            code_num++;
        }
        // Add a zero to the right of the BINARY of bit_rep
        bit_rep = bit_rep << 1;
    }
    table->set = true;

    if(type == LUM_AC_TABLE || type == COL_AC_TABLE){
        /* Fetch an AC bit representation given the int or hex code */ // (O(n) time :( sadly)
        uint8_t fetch_bit = 0xf0;
        uint8_t index = 0;
        for(int i = 0; i < 162; ++i) {
            if(table->symbols[i] == fetch_bit){
                index = i;
                break;
            }
        }
        bit_rep = table->codes[index];
        /* Fetch the length of the huffman symbol */
        uint8_t sum_index = 0;
        uint8_t fetch_index = 0;
        // fetch_bit replaced with is the index of the bit (see DC fetching) (add one to index to counter zero indexing)
        while(sum_index < index+1){
            sum_index += table->offsets[fetch_index];
            fetch_index++;
        }
        fetch_index--;
        table->ZRL_code_index = index; table->ZRL_code_len = fetch_index;

        /* Fetch an AC bit representation given the int or hex code */ // (O(n) time :( sadly)
        fetch_bit = 0x00;
        index = 0;
        for(int i = 0; i < 162; ++i) {
            if(table->symbols[i] == fetch_bit){
                index = i;
                break;
            }
        }
        bit_rep = table->codes[index];
        /* Fetch the length of the huffman symbol */
        sum_index = 0;
        fetch_index = 0;
        // fetch_bit replaced with is the index of the bit (see DC fetching) (add one to index to counter zero indexing)
        while(sum_index < index+1){
            sum_index += table->offsets[fetch_index];
            fetch_index++;
        }
        fetch_index--;
        table->EOB_code_index = index; table->EOB_code_len = fetch_index;
    }
}

void write_buffer(uint16_t *buf, uint8_t *nbit, FILE *fptr, uint16_t code_to_write, uint8_t len){
    code_to_write = code_to_write << (sizeof(code_to_write)*8-len);
    for(int i = 0; i < len; ++i) {
        ((code_to_write & (1 << (16 - 1))) >> (16 - 1)) ? bitset(*buf, 15-*nbit) : bitclear(*buf, 15-*nbit);
        code_to_write = code_to_write << 1;

        // If the last bit written to is 15 (buffer now full, cannot iterate nbit) (nbit is 0 indexed)
        if(*nbit == 15){
            // write buf to file (swap bytes because of endian issues :/ should be changed when serial communicating these values)
            uint16_t tmp = (*buf << 8);
            tmp = tmp | (*buf >> 8);
            fwrite(&tmp, sizeof(tmp), 1, fptr);
            *nbit = 0;
        } else {
            (*nbit)++;
        }
    }
}