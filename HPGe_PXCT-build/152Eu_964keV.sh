num1=1
num2=30
 #num1<num2
 for((i=${num1};i<=${num2};i+=1));
 do
 echo Procesing file_No${i} ....
 ./tele964
 mv ExG4_152Eu_964keV.root ExG4_152Eu_964keV_No${i}.root
 mv ExG4_152Eu_964keV_No${i}.root /mnt/hgfs/out/
 done
cd /mnt/hgfs/out/
rm -f ExG4_152Eu_964keV_all.root
hadd ExG4_152Eu_964keV_all.root ExG4_152Eu_964keV_No*.root
