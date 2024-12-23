
module ram_96x1024 (
    input clk,
    input rst,
    input wena,wenb,
    input [9:0] raddra,raddrb,
    input [9:0] waddra,waddrb,
    input [95:0] dina,dinb,
    output reg [95:0] douta,doutb
);

reg [95:0] data [0:1024];

always @(*) begin
    douta = data[raddra];
    doutb = data[raddrb];
end

integer i;
always@(posedge clk) begin
    if(rst) begin
        for(i=0; i<1024; i=i+1) data[i] <= 0;
    end
    else begin
        if(wena) data[waddra] <= dina;
        if(wenb) data[waddrb] <= dinb;
    end
end

endmodule