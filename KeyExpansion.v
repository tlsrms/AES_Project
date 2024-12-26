`timescale 1ns / 1ps

module KeyExpansion (
    input wire [127:0] key,            // Initial 128-bit key
    input wire [3:0] round,            // Current round number
    output reg [127:0] expanded_key    // Expanded 128-bit key for the round
);

    // Internal signals
    wire [31:0] w0, w1, w2, w3;        // Original key words
    reg [31:0] temp;                   // Temporary word for expansion
    wire [31:0] rcon;                  // Round constant
    reg [31:0] rot_word_out;           // Result of RotWord
    wire [7:0] sbox_out0, sbox_out1, sbox_out2, sbox_out3; // Sbox outputs

    // Split the key into 4 words
    assign w0 = key[127:96];
    assign w1 = key[95:64];
    assign w2 = key[63:32];
    assign w3 = key[31:0];

    // Round constant (Rcon)
    assign rcon = (round == 0) ? 32'h01000000 :
                  (round == 1) ? 32'h02000000 :
                  (round == 2) ? 32'h04000000 :
                  (round == 3) ? 32'h08000000 :
                  (round == 4) ? 32'h10000000 :
                  (round == 5) ? 32'h20000000 :
                  (round == 6) ? 32'h40000000 :
                  (round == 7) ? 32'h80000000 :
                  (round == 8) ? 32'h1b000000 :
                  (round == 9) ? 32'h36000000 : 32'h00000000;


    // Instantiate Sbox modules for each byte
    Sbox sbox0 (.data_in(rot_word_out[31:24]), .data_out(sbox_out0));
    Sbox sbox1 (.data_in(rot_word_out[23:16]), .data_out(sbox_out1));
    Sbox sbox2 (.data_in(rot_word_out[15:8]),  .data_out(sbox_out2));
    Sbox sbox3 (.data_in(rot_word_out[7:0]),   .data_out(sbox_out3));
    
    always @(*) begin
        rot_word_out = {w3[23:0], w3[31:24]};  // Rotate left by 1 byte
        temp[31:24] = sbox_out0; // Apply Sbox to the MSB of RotWord output
        temp[23:16] = sbox_out1; // Apply Sbox to the next byte
        temp[15:8]  = sbox_out2; // Apply Sbox to the next byte
        temp[7:0]   = sbox_out3; // Apply Sbox to the LSB of RotWord output
        temp = temp ^ rcon;      // XOR with Rcon
        expanded_key[127:96] = w0 ^ temp;               // w4
        expanded_key[95:64]  = w1 ^ expanded_key[127:96]; // w5
        expanded_key[63:32]  = w2 ^ expanded_key[95:64];  // w6
        expanded_key[31:0]   = w3 ^ expanded_key[63:32];  // w7
    end

endmodule