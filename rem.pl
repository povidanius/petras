#!/usr/bin/perl 
#Sita utilita removina tuos ^M is "dosiniu" failu.    
# p.daniushis; paralax@freemail.lt; http://ik.su.lt/~pdaniusis
# +370 674 01232				    
$dir   = `pwd`;
@files = `ls $dir`;
print " BEGIN \n";
 foreach $file (@files)
  {
   @splitted = split(/\./,$file);
    foreach $x (@splitted)
    {  
     $ext = $x;
    }
     chomp($ext);
     if ($ext eq 'cpp' || $ext eq 'h')
      {
        print "Apdoroju $file";
	chomp ($file);
	`cat $file | col -bx > temp.txt; mv temp.txt $file`;
      }	
  }    
print "END  \n";