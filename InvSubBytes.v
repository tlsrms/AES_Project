`timescale 1ns / 1ps

module InvSubBytes(
    input wire [127:0] data_in,    // 128-bit input state
    output wire [127:0] data_out   // 128-bit output state
);

    // Instantiate 16 Inv_Sbox modules for parallel processing
    Inv_Sbox inv_sbox0 (.data_in(data_in[127:120]), .data_out(data_out[127:120]));
    Inv_Sbox inv_sbox1 (.data_in(data_in[119:112]), .data_out(data_out[119:112]));
    Inv_Sbox inv_sbox2 (.data_in(data_in[111:104]), .data_out(data_out[111:104]));
    Inv_Sbox inv_sbox3 (.data_in(data_in[103:96]),  .data_out(data_out[103:96]));
    Inv_Sbox inv_sbox4 (.data_in(data_in[95:88]),   .data_out(data_out[95:88]));
    Inv_Sbox inv_sbox5 (.data_in(data_in[87:80]),   .data_out(data_out[87:80]));
    Inv_Sbox inv_sbox6 (.data_in(data_in[79:72]),   .data_out(data_out[79:72]));
    Inv_Sbox inv_sbox7 (.data_in(data_in[71:64]),   .data_out(data_out[71:64]));
    Inv_Sbox inv_sbox8 (.data_in(data_in[63:56]),   .data_out(data_out[63:56]));
    Inv_Sbox inv_sbox9 (.data_in(data_in[55:48]),   .data_out(data_out[55:48]));
    Inv_Sbox inv_sbox10 (.data_in(data_in[47:40]),  .data_out(data_out[47:40]));
    Inv_Sbox inv_sbox11 (.data_in(data_in[39:32]),  .data_out(data_out[39:32]));
    Inv_Sbox inv_sbox12 (.data_in(data_in[31:24]),  .data_out(data_out[31:24]));
    Inv_Sbox inv_sbox13 (.data_in(data_in[23:16]),  .data_out(data_out[23:16]));
    Inv_Sbox inv_sbox14 (.data_in(data_in[15:8]),   .data_out(data_out[15:8]));
    Inv_Sbox inv_sbox15 (.data_in(data_in[7:0]),    .data_out(data_out[7:0]));

endmodule
