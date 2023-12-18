/* verilator lint_off UNUSED */
/* verilator lint_off MULTIDRIVEN */
/* verilator lint_off BLKSEQ */
module gpioemu(                            
    n_reset,
    saddress[15:0], srd, swr,                           
    sdata_in[31:0], sdata_out[31:0], 
    gpio_in[31:0], gpio_latch,          	           
    gpio_out[31:0],                     	
    clk,                                	
    gpio_in_s_insp[31:0]);                 

    input 	clk;
	input 	n_reset;
	input 	[15:0]saddress;         
	input 	srd;                   //Wejścia modułu 
	input 	swr;                    
	input	[31:0]sdata_in;              
	input 	[31:0]gpio_in;              
	input	gpio_latch;             

	output 	[31:0]sdata_out;              
	output	[31:0]gpio_out;              //Wyjścia modułu 
    output  [31:0]gpio_in_s_insp;        

	reg [31:0]gpio_in_s;           	
	reg [31:0]gpio_out_s;          	   
	reg [31:0]sdata_in_s;
	reg	[31:0]sdata_out_s;      	    
	reg [23:0]A1;                  	//Rejestry modułu
    reg [23:0]A2;                 
    reg [31:0]W;                  
    reg [5:0]L;              	    
    reg [31:0]B;                   //Rejestr informujący o stanie zleconej operacji	
    reg [47:0]result;                
    
    reg [47:0]total;
    reg [47:0]part_sum;

	integer i, j;

	
    assign sdata_out = sdata_out_s;
	assign gpio_out = gpio_out_s;
    assign gpio_in_s_insp = gpio_in_s;

//przywrócenie do początkowego stanu wartości rejestrów
always @(negedge n_reset) 
begin
    sdata_in_s <= 32'h0;
    gpio_in_s <= 0;
    gpio_out_s <= 0;
    A1 <= 0;
    A2 <= 0;
    W <= 0;
    B <= 0;
    L = 0;
    result <= 48'h0;
    total= 0;
    part_sum = 0;
end

//maszyna stanów
always @(posedge clk) begin
    case (B)
        32'h0: begin  //Stan początkowy - gotowość do wykonania operacji mnożenia
                result <= 0;
        end
        32'h4: begin  //Stan wykonywania mnożenia na zadanych wartościach rejestrów A1 A2
            W <= 32'h0;
            L = 6'h0; 
            part_sum = 48'h0;
            total = 48'h0;

            for (i = 0; i < 24; i = i + 1) begin
                if (A1[i] == 1'b1) begin
                    part_sum = {24'h0, A2} << i;
                    total = total + part_sum;
                end
            end

            result <= total;

            B <= 32'h8; 
        end
        32'h8: begin   //Stan weryfikacji mieszczenia się wyniku na 32bitach.
            if (result[47:32] != 16'h0) begin
                B <= 32'h16;  //Wynik nie mieści się na 32 bitach.
            end else begin
                B <= 32'h32;    //Wynik mieści się na 32 bitach. 
            end
        end
        32'h16: begin   //Zakończenie mnożenia gdy wynik nie mieści się na 32 bitach.
            B <= 32'h16;
        end
        32'h32: begin   //Stan zliczania jedynyek oraz zapisanie wartości wyniku na odpowiednie rejestry.
        for (i = 0; i < 32; i = i + 1) begin
            if (result[i] == 1'b1) begin
                L = L + 1;
            end
        end
            W <= result[31:0]; 
            B <= 32'h64; 
        end
        32'h64: begin   //Koniec mnożenia, przywrócenie wartości gotowości na rejestrze B
            B <= 32'h0; 
        end
        default: B <= 0;  
    endcase
end

	
	
	//zapis danych do rejestru po sygnale gpio_latch	
	always @(posedge gpio_latch)
	begin
		gpio_in_s <= gpio_in;
	end
			
    // odczyt danych  
    always @(posedge srd) begin
        case (saddress)
            16'h430: sdata_out_s[31:0] <= {8'h0, A1};
            16'h438: sdata_out_s[31:0] <= {8'h0, A2};
            16'h440: sdata_out_s <= W;
            16'h448: sdata_out_s <= {26'h0, L};
            16'h450: sdata_out_s <= B;
            default: sdata_out_s <= 32'h0;
        endcase
    end
		
	// zapis danych na gpio_out_s
    always @(posedge swr) begin
        case(saddress)
            16'h430: A1 <= sdata_in[23:0];
            16'h438: begin
                if (B == 32'h0 || B==32'h16)
                begin
                    A2 <= sdata_in[23:0];  
                    B <= 32'h4;                 //Rozpoczęcie operacji mnożenia przy zapisie na A2
                end
            end
            16'h440: gpio_out_s <= W;
            16'h448: gpio_out_s <= {26'h0, L};
            16'h450: gpio_out_s <= B;
            default: gpio_out_s <= 32'h0;
        endcase
    end
		
		
endmodule