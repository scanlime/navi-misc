v 20040111 1
C 42200 54600 1 0 0 rj45-1.sym
{
T 42200 56500 5 10 1 1 0 0 1
refdes=J1
}
C 47200 54600 1 0 1 rj45-1.sym
{
T 47200 56500 5 10 1 1 0 6 1
refdes=J2
}
N 43100 56200 46300 56200 4
N 43100 56000 46300 56000 4
N 43100 55800 45100 55800 4
N 43100 55200 46300 55200 4
T 42200 54400 9 10 1 0 0 0 1
Ethernet Hub/Switch
T 46600 54400 9 10 1 0 0 0 1
Rio Karma Dock
C 45400 55000 1 180 0 capacitor-1.sym
{
T 44800 54600 5 10 1 1 180 0 1
refdes=C1
T 45100 54500 5 10 1 1 0 0 1
value=100pf
}
C 45100 55700 1 0 0 resistor-1.sym
{
T 45200 55500 5 10 1 1 0 0 1
refdes=R1
T 45600 55500 5 10 1 1 0 0 1
value=56
}
N 46000 55800 46300 55800 4
N 44400 55800 44400 54800 4
N 44400 54800 44500 54800 4
N 45400 54800 45500 54800 4
N 45500 54800 45500 55200 4
T 42200 57600 9 10 1 0 0 0 7
This is my potential fix for the Rio Karma's
ethernet problems. Your mileage may vary,
don't try this at home, etc. These values were
determined experimentally. With this fix,
I can reliably connect (with only one light on)
where before I consistently had the two-lights-on
problem.
C 39300 51800 1 0 0 title-A.sym
T 43400 52700 9 20 1 0 0 0 1
Rio Karma Ethernet Kludge
T 43400 52200 9 10 1 0 0 0 1
ethernet-fix.sch
T 44300 51900 9 10 1 0 0 0 1
1
T 44900 51900 9 10 1 0 0 0 1
1
T 47300 52200 9 10 1 0 0 0 1
A
T 47300 51900 9 10 1 0 0 0 1
Micah Dowty <micah@navi.cx>
