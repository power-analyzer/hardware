% Nathan Zimmerly
% Senior Project
% Output Voltage Conversion

R33 = 11E3;
R37 = 100;
C = .1E-6;
R38 = 51E3;
R39 = 51E3;
Rm = 70;
Radc = 320E3;
Vinp = 120*sqrt(2);

R1 = R33;
R2 = R37;
R3 = R38;
R4 = R39;

Zc = 1/(j*2*pi*60*C)

Vin = ((Vinp/R1)*par(R1,R2))/((par(R1,R2)+Zc))*(par((par(R1,R2)+Zc),par(R3,R4)));

Vmultiplier = ((1/R1)*par(R1,R2))/((par(R1,R2)+Zc))*(par((par(R1,R2)+Zc),par(R3,R4)))*(Radc)/(par(par(R1,R2)+Zc,par(R3, R4)) + Rm + Radc)

VoutApprox = Vinp*Vmultiplier

Vout = Vin*(Radc)/(par(par(R1,R2)+Zc,par(R3, R4)) + Rm + Radc)

magVout = abs(Vout)

function z = par(x,y)
z = (x*y)/(x+y); 
end