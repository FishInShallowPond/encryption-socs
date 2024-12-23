module decompress (
    input clk,
    input rst,
    input [3:0] d,     
    input [7:0] in_data_d1,
    input [31:0] in_data_d4,
    input [79:0] in_data_d10,
    output [95:0] out_data
);

// input reg
integer i;
reg [11:0] in_reg [0:7];
reg [11:0] in_reg10 [0:7];
reg [21:0] mul_out [0:7];
reg [3:0] d_reg;



genvar i1;
generate
    for(i1=0;i1<8;i1=i1+1)
        begin
            always @(posedge clk or posedge rst) begin
                if(rst) begin
                    in_reg[i1] <= 12'd0;
                    d_reg <= 0;
                end
                else begin
                    d_reg <= d - 4'd1;
                    if(d == 4'd1) begin
                        in_reg[i1] <= in_data_d1[i1]?12'd1665:12'd0;
                    end
                    else if(d == 4'd4) begin
                        case(in_data_d4[i1*4+3:i1*4])
                            4'd15:in_reg[i1] <= 12'd3121;
                            4'd14:in_reg[i1] <= 12'd2913;
                            4'd13:in_reg[i1] <= 12'd2705;
                            4'd12:in_reg[i1] <= 12'd2497;
                            4'd11:in_reg[i1] <= 12'd2289;
                            4'd10:in_reg[i1] <= 12'd2081;
                            4'd9:in_reg[i1] <= 12'd1873;
                            4'd8:in_reg[i1] <= 12'd1665;
                            4'd7:in_reg[i1] <= 12'd1456;
                            4'd6:in_reg[i1] <= 12'd1248;
                            4'd5:in_reg[i1] <= 12'd1040;
                            4'd4:in_reg[i1] <= 12'd832;
                            4'd3:in_reg[i1] <= 12'd624;
                            4'd2:in_reg[i1] <= 12'd416;
                            4'd1:in_reg[i1] <= 12'd208;
                            4'd0:in_reg[i1] <= 12'd0;
                            default:in_reg[i1] <= 12'd0;
                        endcase
                    end
                    else begin // d == 10
                        in_reg10[i1] <= in_data_d10[i1*10+9:i1*10];

                    end
                end
            end

            always @(mul_out) begin
                if(d == 4'd10) begin
                    in_reg[i1] <= mul_out[i1][0]? (mul_out[i1] >> 1)+12'd1:(mul_out[i1] >> 1);
                end
            end
        end
endgenerate
always @(*) begin
    for(i=0; i<8; i=i+1) mul_out[i] = (in_reg10[i] * 12'd3329) >> d_reg;
end

assign out_data = {  in_reg[7], in_reg[6], in_reg[5], in_reg[4], 
                     in_reg[3], in_reg[2], in_reg[1], in_reg[0] } ;

endmodule