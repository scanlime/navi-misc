for ($i=0;$i<128;$i++) {
    print "\n\tdt\t" if (!($i & 7));
    print ", 0x" if (!($i & 1));
    print int(sin($i*3.14159*2/128)*3+2);
}
