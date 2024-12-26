`timescale 1ns / 1ps

module stimulus;

    // Inputs for AES module
    reg CLK;
    reg nRST;
    reg ENCDEC;                // 1 for Encryption, 0 for Decryption
    reg START;
    reg [127:0] KEY;           // AES Key
    reg [127:0] TEXTIN;        // Input Text

    // Outputs from AES module
    wire DONE;
    wire [127:0] TEXTOUT;
    
    reg [127:0] result;

    // Instantiate the AES module
    aes_top aes_inst (
        .CLK(CLK),
        .nRST(nRST),
        .ENCDEC(ENCDEC),
        .START(START),
        .KEY(KEY),
        .TEXTIN(TEXTIN),
        .DONE(DONE),
        .TEXTOUT(TEXTOUT)
    );

    always @(posedge DONE) begin
        result = TEXTOUT;
    end
    // Clock generation
    initial CLK = 0;
    always #5 CLK = ~CLK;  // 10ns clock period

    // Test sequence
    initial begin
        // Initialize inputs
        nRST = 0;
        ENCDEC = 0;       
        START = 0;

        // Reset the module
        #20 nRST = 1;

        // Start Encryption
        #10 START = 1; KEY = 128'h000102030405060708090a0b0c0d0e0f; TEXTIN = 128'h00112233445566778899aabbccddeeff;
        #10 START = 0; KEY = 128'hx; TEXTIN = 128'hx;

        #50 START = 1;
        #10 START = 0;

        // Wait before starting Decryption
        #100;
        
        nRST = 0;
        
        #20 nRST = 1;
        
        #10 START = 1; ENCDEC = 1; KEY = 128'h000102030405060708090a0b0c0d0e0f; TEXTIN = result;
        #10 START = 0; ENCDEC = 0; KEY = 128'hx; TEXTIN = 128'hx;
        
        #50 START = 1;
        #10 START = 0;
        
        #400 $stop;
    end

endmodule
