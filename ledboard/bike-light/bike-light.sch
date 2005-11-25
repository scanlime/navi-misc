v 20040111 1
C 57900 48500 1 0 0 mosfet-with-diode-1.sym
{
T 58800 49000 5 10 1 1 0 0 1
refdes=Q1
T 58800 48800 5 10 1 1 0 0 1
value=IRF540
}
C 57300 51300 1 0 0 led-1.sym
{
T 58100 51700 5 10 1 1 0 0 1
refdes=D1
}
C 57300 50700 1 0 0 led-1.sym
{
T 58100 51100 5 10 1 1 0 0 1
refdes=D2
}
C 57300 50100 1 0 0 led-1.sym
{
T 58100 50500 5 10 1 1 0 0 1
refdes=D3
}
N 57300 51500 57100 51500 4
N 57100 50900 57300 50900 4
N 57100 50300 57300 50300 4
N 58200 51500 58500 51500 4
N 58500 50900 58200 50900 4
N 58500 50300 58200 50300 4
C 59100 51300 1 0 0 led-1.sym
{
T 59900 51700 5 10 1 1 0 0 1
refdes=D4
}
C 59100 50700 1 0 0 led-1.sym
{
T 59900 51100 5 10 1 1 0 0 1
refdes=D5
}
C 59100 50100 1 0 0 led-1.sym
{
T 59900 50500 5 10 1 1 0 0 1
refdes=D6
}
N 59100 51500 58900 51500 4
N 58900 50900 59100 50900 4
N 58900 50300 59100 50300 4
N 60000 51500 60300 51500 4
N 60300 50900 60000 50900 4
N 60300 50300 60000 50300 4
C 60900 51300 1 0 0 led-1.sym
{
T 61700 51700 5 10 1 1 0 0 1
refdes=D7
}
C 60900 50700 1 0 0 led-1.sym
{
T 61700 51100 5 10 1 1 0 0 1
refdes=D8
}
C 60900 50100 1 0 0 led-1.sym
{
T 61700 50500 5 10 1 1 0 0 1
refdes=D9
}
N 60900 51500 60700 51500 4
N 60700 50900 60900 50900 4
N 60700 50300 60900 50300 4
N 61800 51500 62100 51500 4
N 62100 50900 61800 50900 4
N 62100 50300 61800 50300 4
N 58500 49500 58500 51500 4
N 60300 49800 60300 51500 4
N 58500 49800 62100 49800 4
N 62100 49800 62100 51500 4
C 57200 52000 1 90 0 resistor-1.sym
{
T 57300 52500 5 10 1 1 0 0 1
refdes=R1
T 57300 52300 5 10 1 1 0 0 1
value=9.1
}
C 59000 52000 1 90 0 resistor-1.sym
{
T 59100 52500 5 10 1 1 0 0 1
refdes=R2
T 59100 52300 5 10 1 1 0 0 1
value=9.1
}
C 60800 52000 1 90 0 resistor-1.sym
{
T 60900 52500 5 10 1 1 0 0 1
refdes=R3
T 60900 52300 5 10 1 1 0 0 1
value=9.1
}
N 57100 50300 57100 52000 4
N 58900 50300 58900 52000 4
N 60700 50300 60700 52000 4
N 57100 52900 57100 53100 4
N 58900 53100 58900 52900 4
N 60700 53100 60700 52900 4
C 57500 47800 1 90 0 resistor-1.sym
{
T 57600 48400 5 10 1 1 0 0 1
refdes=R4
T 57600 48200 5 10 1 1 0 0 1
value=100k
}
N 57400 49000 57400 48700 4
N 57400 47800 57400 46600 4
N 58500 46600 58500 48500 4
C 53400 46700 1 0 0 pic12F629.sym
{
T 56100 49800 5 10 1 1 0 6 1
refdes=U1
}
C 52600 46900 1 90 0 switch-pushbutton-no-1.sym
{
T 53000 47500 5 10 1 1 180 0 1
refdes=S1
}
C 49400 50300 1 270 0 battery-2.sym
{
T 49900 49900 5 10 1 1 0 0 1
refdes=B1
T 49900 49700 5 10 1 1 0 0 1
value=6V
}
N 56400 49000 57900 49000 4
N 56400 49400 56700 49400 4
N 56700 49400 56700 46600 4
C 52700 48400 1 90 0 resistor-1.sym
{
T 52800 49000 5 10 1 1 0 0 1
refdes=R4
T 52800 48800 5 10 1 1 0 0 1
value=100k
}
N 52600 47900 52600 48400 4
N 51800 49400 53400 49400 4
N 52600 48200 53400 48200 4
C 51600 51800 1 270 0 diode-1.sym
{
T 52200 51400 5 10 1 1 0 0 1
refdes=D10
T 52200 51200 5 10 1 1 0 0 1
value=1N4001
}
N 52600 46900 52600 46600 4
N 52600 49300 52600 49400 4
N 51800 51800 51800 53100 4
C 52000 47700 1 90 0 capacitor-1.sym
{
T 51500 48300 5 10 1 1 180 0 1
refdes=C1
T 51500 48100 5 10 1 1 180 0 1
value=0.1uF
}
C 50400 50300 1 270 0 capacitor-2.sym
{
T 50900 49900 5 10 1 1 0 0 1
refdes=C2
T 50900 49700 5 10 1 1 0 0 1
value=22uF
}
N 50600 49400 50600 46600 4
N 49600 49400 49600 46600 4
N 49600 46600 58500 46600 4
N 49600 53100 60700 53100 4
N 49600 50300 49600 53100 4
N 50600 50300 50600 53100 4
N 51800 47700 51800 46600 4
N 51800 48600 51800 50900 4
C 47100 43900 1 0 0 title-B.sym
T 57300 44800 9 15 1 0 0 0 1
3-watt microprocessor controlled bicycle headlight
T 61100 44300 9 10 1 0 0 0 1
A
T 61100 44000 9 10 1 0 0 0 1
Micah Dowty
T 58100 44000 9 10 1 0 0 0 1
1
T 58700 44000 9 10 1 0 0 0 1
1
T 57200 44300 9 10 1 0 0 0 1
bike-light.sch