PXCT simulation

Location on fishtank:
cd /mnt/simulations/pxct/lijie/PXCT_Sim/build

1) cmake -DGeant4_DIR=/mnt/simulations/proton_detector/Geant4/geant4.10.2-install/lib/Geant4-10.2.2 /mnt/simulations/pxct/alex/PXCT_Sim

2) make

3) ./tele vis


Location on my laptop:
VMware Ubuntu can also use these files.

gopxcta
cd /home/sun/app/PXCT-build

1) cmp
cmake -DGeant4_DIR=/home/sun/app/geant4.10.02-install/lib/Geant4-10.2.3 /mnt/hgfs/Geant4_PXCT
cma
cmake -DGeant4_DIR=/home/sun/app/geant4.10.02-install/lib/Geant4-10.2.3 /mnt/hgfs/PXCT_Adams/

2) make
#mv tele tele7um11.5
for 11.5 keV X rays

3) ./tele vis
./LowEnergy_11.5keV.sh
or Run_All_LowEnergy.sh runs all sh files one-by-one.
All generated root files are under /mnt/hgfs/out

tele is configured according to different source geometry
sh are common scripts for all sources. They may need some global replacements.