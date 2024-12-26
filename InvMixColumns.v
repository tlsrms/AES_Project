`timescale 1ns / 1ps

module InvMixColumns (
    input  [127:0] data_in,    // 현재 상태 (128비트 블록)
    output [127:0] data_out    // InvMixColumns 적용 후 새로운 상태
);

    // Galois Field 곱셈 함수
    function [7:0] gmul;
        input [7:0] a, b;
        reg [7:0] p;
        reg [7:0] hi_bit_set;
        reg [7:0] temp_a; // a의 복사본
        reg [7:0] temp_b;
        integer i;
        begin
            p = 0;
            temp_a = a; // 복사본 생성
            temp_b = b;
            for (i = 0; i < 8; i = i + 1) begin
                if (temp_b[0] == 1)
                    p = p ^ temp_a;
                hi_bit_set = temp_a & 8'h80; // 최상위 비트 체크
                temp_a = temp_a << 1;
                if (hi_bit_set != 0)
                    temp_a = temp_a ^ 8'h1b; // AES의 다항식 x^8 + x^4 + x^3 + x + 1
                temp_b = temp_b >> 1;
            end
            gmul = p;
        end
    endfunction

    // InvMixColumns 연산
    genvar col;
    generate
        for (col = 0; col < 4; col = col + 1) begin : mix_columns
            wire [7:0] s0, s1, s2, s3;
            wire [7:0] r0, r1, r2, r3;

            assign s0 = data_in[(127 - col * 8):(120 - col * 8)];
            assign s1 = data_in[(95 - col * 8):(88 - col * 8)];
            assign s2 = data_in[(63 - col * 8):(56 - col * 8)];
            assign s3 = data_in[(31 - col * 8):(24 - col * 8)];

            // MixColumns 변환
            assign r0 = gmul(s0, 8'h0e) ^ gmul(s1, 8'h0b) ^ gmul(s2, 8'h0d) ^ gmul(s3, 8'h09);
            assign r1 = gmul(s0, 8'h09) ^ gmul(s1, 8'h0e) ^ gmul(s2, 8'h0b) ^ gmul(s3, 8'h0d);
            assign r2 = gmul(s0, 8'h0d) ^ gmul(s1, 8'h09) ^ gmul(s2, 8'h0e) ^ gmul(s3, 8'h0b);
            assign r3 = gmul(s0, 8'h0b) ^ gmul(s1, 8'h0d) ^ gmul(s2, 8'h09) ^ gmul(s3, 8'h0e);
            
            assign data_out[(127 - col * 8):(120 - col * 8)] = r0;
            assign data_out[(95 - col * 8):(88 - col * 8)] = r1;
            assign data_out[(63 - col * 8):(56 - col * 8)] = r2;
            assign data_out[(31 - col * 8):(24 - col * 8)] = r3;
        end
    endgenerate

endmodule
