num1=1
num2=40
 #num1<num2
 for((i=${num1};i<=${num2};i+=1));
 do
 echo Procesing file_No${i} ....
 ./tele1213
 mv ExG4_152Eu_1213keV.root ExG4_152Eu_1213keV_No${i}.root
 mv ExG4_152Eu_1213keV_No${i}.root /mnt/hgfs/out/
 done
cd /mnt/hgfs/out/
rm -f ExG4_152Eu_1213keV_all.root
hadd ExG4_152Eu_1213keV_all.root ExG4_152Eu_1213keV_No*.root
