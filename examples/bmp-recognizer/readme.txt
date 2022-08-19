This is raw training data for MLP training system =petras=.

 Training data
  contains 321 15x18 pixel,122 bytes long black-white bitmap 
  files of two classes (similes and not smiles) and prepare.cpp -
  MLP friendly training data generator source code.

 Testing tools
  contains 3 validation sub sets each 11 examples and
  gen.cpp - validation sub set generator source code.
 
 How to use:
  1. compile propare.cpp : $gcc -o prepare prepare.cpp
  2. copy executable into "train-data" directory and execute it.
  3. compile validation set generator: $gcc -o gen gen.cpp
  4. compy it into validation set folder,for example testing-tools/3 and execute.
  5. copy /train-data/smiles.ann and testing-tools/3/validation.set to directory
  what contains =petras= executable.
  6. execute "petras -i"
  7. in MLP contole type:
	petra$ t smiles.ann
	(this trains neural network form smiles.ann)
	wait after succes is returned and then type
	petra$ sim 1 validation.set
	and you will get 11 outputs for each example from validation set.
  	
(C) Povilas Daniushis, pdaniusis@ik.su.lt
	
	
	
	
	
	
	
  
  
  