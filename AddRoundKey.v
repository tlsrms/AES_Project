`timescale 1ns / 1ps

module AddRoundKey (
    input wire [127:0] data_in,       // Input state (128 bits)
    input wire [127:0] round_key,     // Current round key (128 bits)
    output wire [127:0] data_out      // Output state (128 bits)
);

    wire [127:0] transformed_round_key;
    assign transformed_round_key = text_to_state(round_key);
    assign data_out = data_in ^ transformed_round_key;
    
 function [127:0] text_to_state(input [127:0] text);
        integer i, j;
        reg [7:0] state [0:15]; // 1차원 배열로 state 선언
        reg [127:0] result;
        begin
            // 열 기준으로 변환하여 1차원 배열에 저장
            for (i = 0; i < 4; i = i + 1) begin
                for (j = 0; j < 4; j = j + 1) begin
                    state[i * 4 + j] = text[127 - ((j * 4 + i) * 8) -: 8];
                end
            end

            // 1차원 배열을 128비트로 병합
            for (i = 0; i < 16; i = i + 1) begin
                result[127 - (i * 8) -: 8] = state[i];
            end

            text_to_state = result;
        end
    endfunction
    
    
endmodule
