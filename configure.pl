#!/usr/bin/perl
#
# This is script for source code configuration.
# You must runn this before you install petras.
# And of course you must have perl interpreter installed on your system.
#

 print "Perceptron trainin system --petras-- source code configuration \n\n";
 while (!($os eq 'UNIX') && !($os eq 'Windows'))
  {
    print "0. You are UNIX or Windows user ? \n";
    $os = <STDIN>;
    chomp($os);
  }    
 if ($os eq 'UNIX') 
  {
   $code = "#define UNIX \n";
  }
  while (!$name)
   {
    print "1. What is your name ? \n";
    $name = <STDIN>;
    chomp($name);
   }
 unless (open(FILE,">>config.h"))
  {
   die ("cant open config.h \n");
  } 
  print FILE "$code";
  close(FILE);
  print "Configuration is completed.Now run `make install`";