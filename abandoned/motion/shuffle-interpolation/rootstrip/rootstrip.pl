#!/usr/bin/perl -w

my $INFILE;
my $ROOTFILE;

sub print_usage()
{
    print "usage: rootstrip -h|--help\n";
    print "          prints this message\n";
    print "       rootstrip infile\n";
    print "          strips root data out of infile and outputs\n";
    print "          result to stdout.\n";
    print "       rootstrip -i rootfile infile\n";
    print "          overwrites root data of infile with root\n";
    print "          rootfile root dadta and outputs to stdout\n";
    
    exit;
}


sub standard()
{
    open(FILE,$INFILE);
    
# send out first stuff
    for(my $i = 0; $i < 3; $i++) {
        my $junk = <FILE>;
        chomp $junk;
        print "$junk\n";
    }
    
    my $i = 0;
    while(<FILE>) {
        if($i == 30)
        {
            $i = 0;
        }
        if($i == 1)
        {
            print "root 0.0 0.0 0.0 0.0 0.0 0.0\n"
        }
        else
        {
            chomp $_;
            print "$_\n";
        }
        $i++;
        
    }
    
}

sub insert()
{
    open(INFILE,$INFILE);
    open(ROOTFILE,$ROOTFILE);
# pass over first stuff
    for(my $i = 0; $i < 3; $i++) {
        my $rootjunk = <ROOTFILE>;
        my $injunk = <INFILE>;
        chomp $injunk;
        print "$injunk\n";
    }
    
    my @indata;
    my @rootdata;
    my $i = 0;
    while(<INFILE>)
    {
        chomp $_;
        $indata[$i] = $_;
        $i++;
    }
    
    $i = 0;
    while(<ROOTFILE>)
    {
        chomp $_;
        $rootdata[$i] = $_;
        $i++;
    }
    
    my $max;
    if($#rootdata < $#indata)
    {
        $max = $#rootdata;
    }
    else
    {
        $max = $#indata;
    }
    
    $i = 0;
    my $j;
    for($j = 0; $j < $max; $j++)
    {
        if($i == 30)
        {
            $i = 0;
        }
        if($i == 1)
        {
            print "$rootdata[$j]\n";
        }
        else
        {
            print "$indata[$j]\n";
        }
        $i++;
        
    }
}

################################################
################ entry point ###################
################################################
if($#ARGV <= -1 || $ARGV[0] eq "-h" || $ARGV[0] eq "--help")
{
    print_usage();
}

elsif($ARGV[0] eq "-i" && $#ARGV == 2)
{
    $ROOTFILE = $ARGV[1];
    $INFILE = $ARGV[2];
    insert();
}

elsif($#ARGV == 0)
{
    $INFILE = $ARGV[0];
    standard();
}

else
{
    print_usage();
}






