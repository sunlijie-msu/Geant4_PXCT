num1=1
num2=30
 #num1<num2
 for((i=${num1};i<=${num2};i+=1));
 do
 echo Procesing file_No${i} ....
 ./tele122
 mv ExG4_152Eu_122keV.root ExG4_152Eu_122keV_No${i}.root
 mv ExG4_152Eu_122keV_No${i}.root /mnt/hgfs/out/
 done
cd /mnt/hgfs/out/
rm -f ExG4_152Eu_122keV_all.root
hadd ExG4_152Eu_122keV_all.root ExG4_152Eu_122keV_No*.root
