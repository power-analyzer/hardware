% Nathan Zimmerly
% Senior Project
% Output Voltage Conversion

R33 = 110E3;  % 110E3 calculated, 100.9k actual
R37 = 100;     % 100 calculated, 99.4 actual
C = 10E-6;      % C2: .1E-6 calculated, actual 8.43u (using a 10uF) 
R38 = 51E3;     % 51E3 calculated, 37.02k actual bad from capacitance
R39 = 51E3;     % 51E3 calculated, 34.57k actual bad from capacitance
Rm = 70;
Radc = 320E3;
Vinp = 120*sqrt(2);

R1 = R33;
R2 = R37;
R3 = R38;
R4 = R39;

Zc = 1/(j*2*pi*60*C);

Vin = ((Vinp/R1)*par(R1,R2))/((par(R1,R2)+Zc))*(par((par(R1,R2)+Zc),par(R3,R4)));

Vmultiplier = ((1/R1)*par(R1,R2))/((par(R1,R2)+Zc))*(par((par(R1,R2)+Zc),par(R3,R4)))*(Radc)/(par(par(R1,R2)+Zc,par(R3, R4)) + Rm + Radc)

VoutApprox = Vinp*Vmultiplier;

v120 = .15/Vmultiplier

magv120 = abs(v120)

Vout = Vin*(Radc)/(par(par(R1,R2)+Zc,par(R3, R4)) + Rm + Radc);

magVout = abs(Vout);

function z = par(x,y)
z = (x*y)/(x+y); 
end