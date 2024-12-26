`timescale 1ns / 1ps

module ShiftRows (
    input wire [127:0] data_in,    // 128-bit input state
    output wire [127:0] data_out   // 128-bit output state
);

    wire [7:0] s [15:0];
    assign {s[0],  s[1],  s[2],  s[3],
            s[4],  s[5],  s[6],  s[7],
            s[8],  s[9],  s[10], s[11],
            s[12], s[13], s[14], s[15]} = data_in;
            
    assign data_out = { s[0],  s[1],  s[2], s[3],  // Row 0
                    s[5],  s[6],  s[7], s[4],   // Row 1
                    s[10],  s[11], s[8],  s[9],   // Row 2
                    s[15], s[12],  s[13],  s[14]}; // Row 3

endmodule

