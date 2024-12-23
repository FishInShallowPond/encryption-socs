`include "../Hash/KECCAK_p.v"
//H
module SHA3_256(input [0:12544-1] M,//KYBER512_PK: 6400b; KYBER768_PK: 9472b; KYBER1024_PK: 12544b;
                                    //KYBER512_C: 6144b; KYBER768_C: 8704b; KYBER1024_C: 12544b;
                                    //M: 256b
                input active, //Let stage go to s1 
                input clk, rst,
                input [1:0] kyber_mode,H_mode,
                output reg finish,
                output [0:256-1] Z_rv);

// kyber_mode define
parameter kyber512 = 2'd0;
parameter kyber768 = 2'd1;
parameter kyber1024 = 2'd2;
// H_mode define
parameter H_mode_M = 2'd0;
parameter H_mode_PK = 2'd1;
parameter H_mode_C = 2'd2;

parameter d_SIZE = 256,
          c_SIZE = 512,
          r_SIZE = 1088, //r=b-c
          M_SIZE_PK_KYBER512 = 6400,
          M_SIZE_PK_KYBER768 = 9472,
          M_SIZE_PK_KYBER1024 = 12544,
          M_SIZE_C_KYBER512 = 6144,
          M_SIZE_C_KYBER768 = 8704,
          M_SIZE_C_KYBER1024 = 12544,
          M_SIZE_M = 256;
parameter j_PK_KYBER512 = 124,      //j_PK_KYBER512     =6*r_SIZE - (M_SIZE_PK_KYBER512+2+2)
          j_PK_KYBER768 = 316,      //j_PK_KYBER768     =9*r_SIZE - (M_SIZE_PK_KYBER768+2+2)
          j_PK_KYBER1024 = 508,     //j_PK_KYBER1024    =12*r_SIZE - (M_SIZE_PK_KYBER1024+2+2)
          j_C_KYBER512 = 380,       //j_C_KYBER512      =6*r_SIZE - (M_SIZE_C_KYBER512+2+2)
          j_C_KYBER768 = 1084,      //j_C_KYBER768      =9*r_SIZE - (M_SIZE_C_KYBER768+2+2)
          j_C_KYBER1024 = 508,      //j_C_KYBER1024     =12*r_SIZE - (M_SIZE_C_KYBER1024+2+2)
          j_M = 828;                //j_M               =r_SIZE - (M_SIZE_M+2+2)

wire [0:12544-1] M_rv;
wire [0:(M_SIZE_PK_KYBER512+2)-1] N_PK_KYBER512;
wire [0:(M_SIZE_PK_KYBER768+2)-1] N_PK_KYBER768;
wire [0:(M_SIZE_PK_KYBER1024+2)-1] N_PK_KYBER1024;
wire [0:(M_SIZE_C_KYBER512+2)-1] N_C_KYBER512;
wire [0:(M_SIZE_C_KYBER768+2)-1] N_C_KYBER768;
wire [0:(M_SIZE_C_KYBER1024+2)-1] N_C_KYBER1024;
wire [0:(M_SIZE_M+2)-1] N_M;
wire [4:0] nr;
wire [0:1599] str_out;
wire [3:0] cnt;//counter+1
wire [11:0] counter_r_r_SIZE;//counter_r-r_SIZE
reg [0:256-1] Z;
reg [2:0] cs,ns;
reg string_val;
reg [0:1599] str, str_temp, str_temp2;
reg [3:0] counter;  //0~n-1
reg [4:0] counter_f;//count rnds of KECCAK_p
reg [11:0] counter_r;//count times of Z=Z||Trunc_r(S)
reg [0:(r_SIZE*2)-1] Z_temp;// r*2 bits
reg [0:13055] P_reg;
reg [3:0] n = 4'd12;

genvar g;
generate
    for ( g = 0; g < 1568; g=g+1 ) begin
        assign M_rv[8*g+0] = M[8*g+7];
        assign M_rv[8*g+1] = M[8*g+6];
        assign M_rv[8*g+2] = M[8*g+5];
        assign M_rv[8*g+3] = M[8*g+4];
        assign M_rv[8*g+4] = M[8*g+3];
        assign M_rv[8*g+5] = M[8*g+2];
        assign M_rv[8*g+6] = M[8*g+1];
        assign M_rv[8*g+7] = M[8*g+0];
    end 
endgenerate

generate
    for ( g = 0; g < 32; g=g+1 ) begin
        assign Z_rv[8*g+0] = Z[8*g+7];
        assign Z_rv[8*g+1] = Z[8*g+6];
        assign Z_rv[8*g+2] = Z[8*g+5];
        assign Z_rv[8*g+3] = Z[8*g+4];
        assign Z_rv[8*g+4] = Z[8*g+3];
        assign Z_rv[8*g+5] = Z[8*g+2];
        assign Z_rv[8*g+6] = Z[8*g+1];
        assign Z_rv[8*g+7] = Z[8*g+0];
    end 
endgenerate

assign N_PK_KYBER512 ={M_rv[6144:12543],2'b01}; //N=M||01
assign N_PK_KYBER768 ={M_rv[3072:12543],2'b01};
assign N_PK_KYBER1024 ={M_rv,2'b01};
assign N_C_KYBER512 ={M_rv[6400:12543],2'b01};
assign N_C_KYBER768 ={M_rv[3840:12543],2'b01};
assign N_C_KYBER1024 ={M_rv,2'b01};
assign N_M ={M_rv[12288:12543],2'b01};
assign nr = 5'd24;   //nr=24

assign cnt = counter + 3'd1;//counter+1
assign counter_r_r_SIZE = counter_r-r_SIZE;

parameter s0 = 3'b000,
          s1 = 3'b001,
          s2 = 3'b010,
          s3 = 3'b011,
          s4 = 3'b100,
          s5 = 3'b101,
          s6 = 3'b110,
          s7 = 3'b111;

KECCAK_p keccak_p( .S(str_temp),
                   .nr(nr),
                   .string_val(string_val),
                   .clk(clk), .rst(rst),
                   .S_out(str_out));
always@(*)begin
    case({H_mode,kyber_mode})
        {H_mode_PK,kyber512}: begin
            P_reg = {N_PK_KYBER512,1'b1,{j_PK_KYBER512{1'b0}},1'b1};
            counter = 6;
        end
        {H_mode_PK,kyber768}: begin
            P_reg = {N_PK_KYBER768,1'b1,{j_PK_KYBER768{1'b0}},1'b1};
            counter = 3;
        end
        {H_mode_PK,kyber1024}: begin
            P_reg = {N_PK_KYBER1024,1'b1,{j_PK_KYBER1024{1'b0}},1'b1};
            counter = 0;
        end
        {H_mode_C,kyber512}: begin
            P_reg = {N_C_KYBER512,1'b1,{j_C_KYBER512{1'b0}},1'b1};
            counter = 6;
        end
        {H_mode_C,kyber768}: begin
            P_reg = {N_C_KYBER768,1'b1,{j_C_KYBER768{1'b0}},1'b1};
            counter = 3;
        end
        {H_mode_C,kyber1024}: begin
            P_reg = {N_C_KYBER1024,1'b1,{j_C_KYBER1024{1'b0}},1'b1};
            counter = 0;
        end
        default: begin
            P_reg = {N_M,1'b1,{j_M{1'b0}},1'b1};
            counter = 11;
        end
    endcase
end

always@(posedge clk or posedge rst)begin
        if(rst)begin
            cs <= s0;
            str_temp <= 1600'd0;
            str_temp2 <= 1600'd0;
            counter <= 4'd0;
            counter_f <= 5'd0;
            counter_r <= 12'd0;
            Z_temp <= {r_SIZE*3{1'b0}};

        end
        else begin
            cs <= ns;
            //str_temp= S^(Pi||c(0))
            if(ns == s1)begin
                str_temp <= str^{P_reg[r_SIZE*counter+:r_SIZE],{c_SIZE{1'b0}}};
            end
            else if(ns == s4) begin
                str_temp <= str_temp2;
            end
            else begin
                str_temp <= str_temp;
            end
            
            
            //let S equal to str_out after do a S=f(S^(Pi||c(0))) and S=f(S)
            str_temp2 <= (cs == s2 || cs == s5)? str_out:str_temp2;

            counter <= (cs == s2)? counter+3'd1 : counter;
            counter_r <= (ns == s3 || ns==s6)? counter_r+r_SIZE:counter_r;//+r_SIZE
            
            //counter_f
            if((cs == s1)||(cs == s4))begin
                counter_f <= counter_f+5'd1;
            end
            else if((ns == s1) || (ns==s4))begin
                counter_f <= 5'd0;
            end
            else begin
                counter_f <= counter_f;
            end

            //Z=Z||Trunc_r(S)
            if(cs == s3 || cs == s6)begin
                Z_temp[counter_r_r_SIZE +:r_SIZE] <= str_temp2[0:r_SIZE-1];
            end
            else begin
                Z_temp[counter_r_r_SIZE +:r_SIZE] <= Z_temp[counter_r_r_SIZE +:r_SIZE];
            end

        end

end
            


always@(cs or active or counter_f or cnt or counter_r)begin
	//next stage
	case(cs)
		s0: ns = (active)? s1:s0;
		s1: ns = (counter_f== 5'd25) ? s2:s1;//S=f(S^(Pi||c(0)))
		s2: ns = (cnt == n) ? s3:s1; //counter is from 0 to n-1
        s3: ns = (counter_r >= d_SIZE)?s7:s4;//determine if d<=|Z|
        s4: ns = (counter_f == 5'd25) ?s5:s4;//S=f(S)
        s5: ns = s6;
        s6: ns = (counter_r >= d_SIZE)?s7:s4;//determine if d<=|Z|
        s7: ns = s0;//output
		default: ns = s0;
	endcase	
end

always@(cs or counter or str_temp2)begin
    case(cs)
		s0: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = 1600'd0;
                finish = 1'b0;
            end    
		s1: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                finish = 1'b0;
                if(counter==3'd0)begin
                    str = 1600'd0;
                end
                else begin
                    str = str_temp2;
                end
            end
		s2: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s3: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
        s4: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s5: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s6: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
        s7: begin
                Z = Z_temp[0:d_SIZE-1]; //Z=Trunc_d(Z)
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b1;
            end
		default: begin
                Z = {d_SIZE{1'd0}};
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
    endcase
end

endmodule