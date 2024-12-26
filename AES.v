`timescale 1ns / 1ps

module aes_top (
    input wire CLK,             // Clock
    input wire nRST,            // Active-low Reset
    input wire ENCDEC,          // 0: Encryption, 1: Decryption
    input wire START,           // Start signal
    input wire [127:0] KEY,     // 128-bit Key
    input wire [127:0] TEXTIN,  // 128-bit Input Text
    output reg DONE,            // Operation complete signal
    output reg [127:0] TEXTOUT  // 128-bit Output Text
);

    // Internal signals and states
    reg [127:0] state;               // Current AES state
    reg [4:0] round;                 // Round counter
    reg [3:0] inv_round;
    reg [127:0] round_key;           // Current round key
    wire [127:0] sub_bytes_out;      // Output of SubBytes
    wire [127:0] shift_rows_out;     // Output of ShiftRows
    wire [127:0] mix_columns_out;    // Output of MixColumns
    wire [127:0] add_round_key_out;  // Output of AddRoundKey
    wire [127:0] expanded_key;       // Expanded key for the current round
    wire [127:0] inv_expanded_key;
    wire [127:0] inv_sub_bytes_out;  // Output of InvSubBytes
    wire [127:0] inv_shift_rows_out; // Output of InvShiftRows
    wire [127:0] inv_mix_columns_out;// Output of InvMixColumns

    // Parameters for state machine
    localparam IDLE = 3'b000, ENC_PROCESS = 3'b001, ENC_COMPLETE = 3'b010,
               DEC_READY = 3'b011, DEC_PROCESS = 3'b100, DEC_COMPLETE = 3'b101;
    reg [2:0] current_state = IDLE, next_state = IDLE; 
    reg [127:0] round_key_next;
    reg is_DEC = 0, busy = 0;

    // Instantiate SubModules
    SubBytes u_subbytes (.data_in(state), .data_out(sub_bytes_out));
    ShiftRows u_shiftrows (.data_in(sub_bytes_out), .data_out(shift_rows_out));
    MixColumns u_mixcolumns (.data_in(shift_rows_out), .data_out(mix_columns_out));
    KeyExpansion u_keyexpansion (.key(round_key), .round(round), .expanded_key(expanded_key));
    InvShiftRows u_invshiftrows (.data_in(state), .data_out(inv_shift_rows_out));
    InvSubBytes u_invsubbytes (.data_in(inv_shift_rows_out), .data_out(inv_sub_bytes_out));
    InvMixColumns u_invmixcolumns (.data_in(add_round_key_out), .data_out(inv_mix_columns_out));
    InvKeyExpansion u_invkeyexpansion (.key(round_key), .round(inv_round), .inv_expanded_key(inv_expanded_key));
    AddRoundKey u_addroundkey (
        .data_in((is_DEC == 0) ? ((round == 0) ? state : (round == 10) ? shift_rows_out : mix_columns_out) :
                                 ((inv_round == 10) ? state : inv_sub_bytes_out)),
        .round_key(round_key),
        .data_out(add_round_key_out)
    );
    
    always @(posedge CLK or negedge nRST) begin
            if (!nRST) begin
                current_state <= IDLE;
                round <= 4'd0;
                DONE <= 1'b0;
                busy <= 1'b0;
            end else begin
                current_state <= next_state;
                case (current_state)
                    IDLE: begin
                        if(START && !busy) begin
                            round_key <= KEY;
                            state <= text_to_state(TEXTIN);
                            is_DEC <= ENCDEC;
                            busy <= 1'b1;
                        end
                        TEXTOUT <= 128'hx;
                        round <= 4'd0;
                        inv_round <= 4'd0;
                        DONE <= 1'b0;
                    end
                    ENC_PROCESS: begin
                        if (round == 0) begin
                            // 첫 번째 AddRoundKey 단계
                            state <= add_round_key_out;
                            round <= round + 1;
                            round_key <= expanded_key;
                        end else if (round < 10) begin
                            // 중간 라운드 처리MixColumns
                            state <= add_round_key_out;          
                            round <= round + 1;                  // 다음 라운드로 이동
                            round_key <= expanded_key;
                        end else begin
                            // 마지막 라운드 처리 (MixColumns 생략)
                            state <= add_round_key_out;          
                            round <= 4'd0;
                            round_key <= expanded_key;
                        end
                    end
                    ENC_COMPLETE: begin
                        TEXTOUT <= text_to_state(state); // 최종 결과 출력
                        DONE <= 1'b1;
                        busy <= 1'b0;
                    end
                    DEC_READY: begin
                        if(inv_round < 10) begin
                            round <= round + 1;
                            inv_round <= inv_round + 1;
                            round_key <= expanded_key;
                        end
                    end
                    DEC_PROCESS: begin
                        if (inv_round == 10) begin
                            // 첫 번째 AddRoundKey 단계 (라운드 10의 키 사용)
                            state <= add_round_key_out;
                            round <= round + 1;
                            inv_round <= inv_round - 1;
                            round_key <= inv_expanded_key;
                             // 다음 라운드 키 생성 (라운드 9)
                        end else if (inv_round < 10 && inv_round > 0) begin
                            // 중간 라운드 처리
                            state <= inv_mix_columns_out;
                            round <= round + 1;                  // 다음 라운드로 이동
                            inv_round <= inv_round - 1;
                            round_key <= inv_expanded_key;
                        end else begin
                            state <= add_round_key_out; 
                        end
                    end
                    DEC_COMPLETE: begin
                        TEXTOUT <= text_to_state(state); // 최종 결과 출력
                        DONE <= 1'b1;
                        busy <= 1'b0;
                    end
                endcase
            end
        end
    
    always @(*) begin
        case (current_state)
            IDLE: begin
                if (ENCDEC == 0 && START == 1 && busy == 0)
                    next_state = ENC_PROCESS;
                else if (ENCDEC == 1 && START == 1 && busy == 0)
                    next_state = DEC_READY;
            end
            ENC_PROCESS: begin
                if (round == 10)
                    next_state = ENC_COMPLETE;
            end
            ENC_COMPLETE: next_state = IDLE;
            DEC_READY: begin
                if(inv_round == 10)
                    next_state = DEC_PROCESS;
            end
            DEC_PROCESS: begin
                if (inv_round == 0)
                    next_state = DEC_COMPLETE;
            end
            DEC_COMPLETE: next_state = IDLE;
        endcase
    end

    function [127:0] text_to_state(input [127:0] text);
        integer i, j;
        reg [7:0] temp [0:15]; // 1차원 배열로 state 선언
        reg [127:0] result;
        begin
            // 열 기준으로 변환하여 1차원 배열에 저장
            for (i = 0; i < 4; i = i + 1) begin
                for (j = 0; j < 4; j = j + 1) begin
                    temp[i * 4 + j] = text[127 - ((j * 4 + i) * 8) -: 8];
                end
            end

            // 1차원 배열을 128비트로 병합
            for (i = 0; i < 16; i = i + 1) begin
                result[127 - (i * 8) -: 8] = temp[i];
            end

            text_to_state = result;
        end
    endfunction
endmodule
