`timescale 1ns/10ps
module gpioemu_tb;

reg n_reset = 0;
reg [15:0] saddress = 0;
reg srd = 0;
reg swr = 0;
wire [31:0] sdata_out;
reg [31:0] sdata_in = 0;
reg [31:0] gpio_in = 0;
reg gpio_latch = 0;
reg clk = 0;
wire [31:0] gpio_out;
wire [31:0] gpio_in_s_insp;

initial begin
    $dumpfile("gpioemu.vcd");
    $dumpvars(0,gpioemu_tb);
end

initial begin
    clk = 0;
    forever
    begin
        # 5 clk = ~clk;
    end
end

initial begin

#5 n_reset = 1;


#5 sdata_in = 24'h2137;
#5 gpio_latch = 1;
#5
#5 gpio_latch = 0;
#10


#5 saddress = 16'h430;
#5 swr = 1;
#5
#5 swr = 0;
#10;


// Odczyt z rejestru A1
#5 saddress = 16'h430;
#5 srd = 1;
#5
#5 srd = 0;
#10;

// zmiana wartosci
#5 sdata_in = 24'h125;
#5 gpio_latch = 1;
#5
#5 gpio_latch = 0;
#10

// Zapis do rejestru A2 --- pierwsze mnozenie
#5 saddress = 16'h438;
#5 swr = 1;
#5
#5 swr = 0;
#10;

// Odczyt z rejestru A2
#5 saddress = 16'h438;
#5 srd = 1;
#5
#5 srd = 0;
#10;

// Odczyt z rejestru B
#5 saddress = 16'h450;
#5 srd = 1;
#5
#5 srd = 0;
#10;


#100;

// Odczyt z rejestru W
#5 saddress = 16'h440;
#5 srd = 1;
#5
#5 srd = 0;
#10;

#5 saddress = 16'h440;
#5 swr = 1;
#5
#5 swr = 0;
#10

#5 saddress = 16'h448;
#5 swr = 1;
#5
#5 swr = 0;
#10;
// Odczyt z rejestru licznika operacji
#5 saddress = 16'h458;
#5 srd = 1;
#5
#5 srd = 0;
#10;

// ustawienie wartosci
#5 sdata_in = 24'hf3ef67;
#5 gpio_latch = 1;
#5
#5 gpio_latch = 0;
#10

// Zapis do rejestru A1
#5 saddress = 16'h430;
#5 swr = 1;
#5
#5 swr = 0;
#10;

// Odczyt z rejestru A1
#5 saddress = 16'h430;
#5 srd = 1;
#5
#5 srd = 0;
#10;

// zmiana wartosci
#5 sdata_in = 24'hf1f5fe;
#5 gpio_latch = 1;
#5
#5 gpio_latch = 0;
#10

// Zapis do rejestru A2 --- drugie mnozenie
#5 saddress = 16'h438;
#5 swr = 1;
#5
#5 swr = 0;
#200;

#5 saddress = 16'h440;
#5 srd = 1;
#5
#5 srd = 0;

#5 saddress = 16'h440;
#5 swr = 1;
#5 
#5 swr = 0;

#5 saddress = 16'h448;
#5 srd = 1;
#5 
#5 srd = 0;
#100
$finish;
end
gpioemu emu1 (n_reset, saddress, srd, swr, sdata_in, sdata_out, gpio_in, gpio_latch, gpio_out, clk, gpio_in_s_insp);

endmodule