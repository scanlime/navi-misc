/*
 * Obfuscated 99 bottles of beer on the wall.
 * -- (C) 2003, Micah Dowty <micah@navi.cx>
 *
 * Will only work on x86-linux machines. Tested 
 * successfully on gcc 3.2, 3.3, and 4.0. Tested
 * unsuccessfully on gcc 3.4. If you want to see
 * the results but your compiler isn't supported,
 * a binary has been included. Do not compile with
 * optimization enabled!
 */

               main(int,...);
               char *i,*o;int
               n;void d(char*
               z){z[1]&15?z[1
               ]--:(z[1]=0x39
               )&&(*z)--;*z=*
               z&15?*z: (z[1]
               ==0x31?z[9]=0:
               0),0;} void c(
               char *z){ z[1]
              ==0x30&&!*z?(z+=
             36):0;*z?main(*z),
            c(z+1):z[1]?c(z+1):0
           ;}void v(char *z){c(z+
          1);d(z+1);d(z+0x21);d(z+
         0x54);if(z[2]!=0x30||z[1])
        v(z); }void p(int z,...) {((
       void( *)())&z)();if(!((z%5)-1)
      )v(&z); }main(int x,...){ if(!x)
     return;main(0, 61,195, 128,205,66,
     4, 36, 76, 141, 4,176,210, 49,219,
     49, 192,  49);p(n^=2033238154, n^=
     -1320603213,  n^=119210008,    n^=
     1901263895,n^=-1014969081);for(i=o
     =main;*i!=49;)i++;for(; (*o= *i)!=
     -61;i+=8,o++);p(n^=1414354593, n^=
     1409747470 , n ^= 1112736515 , n^=
     1112670986 , n ^= 1414662922 , n^=
     50334471   , n ^= 1531709705 , n^=
     206919019  , n ^= 1414354536 , n^=
     1409747470 , n ^= 1112736515 , n^=
     1313997578 , n ^= 1192440175 , n^=
     84833391   , n ^= 17498112   , n^=
     1330138642 , n ^= 1398738695 , n^=
     1392969808 , n ^= 1427839809 , n^=
     2135102991 , n ^= 1745640535 , n^=
     240405590  , n ^= 55838474   , n^=
     172118787  , n ^= 172118531  , n^=
     122966539  , n ^= 151191563  , n^=
     1801014281 , n ^= 170814566  , n^=
     1830842180 , n ^= 1296375073 , n^=
     1410407693 , n ^= 1409747470 , n^=
     1112736515 , n ^= 1112670986 , n^=
     1414662922 , n ^= 50334471   , n^=
      1498155273, n ^= 778857579  ,n^=
          28367277 ^ 2080381334);}
