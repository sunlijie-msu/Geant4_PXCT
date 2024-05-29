num1=1
num2=30
 #num1<num2
 for((i=${num1};i<=${num2};i+=1));
 do
 echo Procesing file_No${i} ....
 ./tele1173
 mv ExG4_60Co_1173keV.root ExG4_60Co_1173keV_No${i}.root
 mv ExG4_60Co_1173keV_No${i}.root /mnt/hgfs/out/
 done
cd /mnt/hgfs/out/
rm -f ExG4_60Co_1173keV_all.root
hadd ExG4_60Co_1173keV_all.root ExG4_60Co_1173keV_No*.root
