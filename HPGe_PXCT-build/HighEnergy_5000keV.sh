num1=51
num2=80
 #num1<num2
 for((i=${num1};i<=${num2};i+=1));
 do
 echo Procesing file_No${i} ....
 ./tele5000
 mv ExG4_HighEnergy_5000keV.root ExG4_HighEnergy_5000keV_No${i}.root
 mv ExG4_HighEnergy_5000keV_No${i}.root /mnt/hgfs/out/
 done
cd /mnt/hgfs/out/
rm -f ExG4_HighEnergy_5000keV_all.root
hadd ExG4_HighEnergy_5000keV_all.root ExG4_HighEnergy_5000keV_No*.root
