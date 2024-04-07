import ensdf_support as es

output_file = '73Kr_decay_scheme.txt'

'''
XRAY DATA

logft tool results:
EC to ground state will have 10:1:0.5 K:L:M
EC to 5890 is about 9:1:0.2 K:L:M

Br kb/k ais 0.15 per https://www.tandfonline.com/doi/pdf/10.1016/j.jrras.2017.04.003
The elements near Br in  Slivensky and Ebert are of a similar value, and S&E reported 0.16 for Se.
In Se, the same K, L, and M shell vacancies should be present as in Br.

For now, we ignore L and M shell vacancies because these are infrequently populated and their energies are different
enough from the K-shell xrays that they shouldn't contribute additional background.

Characteristic x-ray energies from https://physics.nist.gov/PhysRefData/XrayTrans/Html/search.html

Note: currently working to replace x-ray spectrum with EADL using Geant4's built in funcitonality

Energy levels and branching ratios for 73Kr EC followed by gamma decay
are from ENSDF file.

'''

br_xray_prob = 0.85 #probability xray form EC decay to unbound state come from Br instead of Se
br_kb_to_ka = 0.1573 #Hansen et. al.
se_kb_to_ka = 0.1511
#Se xrays will only be present if Br nucleus is above the 3469 keV energy level
ec_xray_configs = [[float('inf'), [11.87775, br_xray_prob,  13.250, br_xray_prob*br_kb_to_ka,\
                                   11.181, (1-br_xray_prob), 12.500, (1-br_xray_prob)*se_kb_to_ka]], \
                   [3500, [11.87775, br_xray_prob,  13.250, br_xray_prob*br_kb_to_ka]]] #Bromine xrays from proton bound state
ic_xray_config = [11.87775, 1, 13.25, br_kb_to_ka] #need to update to include kbetas
ic_electron_config = [14.326, 1.]
#energy for 72Se relative to 73Br ground states is calculated by subtracking Q(e-p) from Q(e) given in ENSF
Br73_proton_config = {'isotope':'72SE', 'ground energy':7096.-4028, 'energies':[0, 860], 'weights': [80, 20]}
es.convert_file('73Kr_EC_DECAY.ensdf', output_file, ec_xray_configs=ec_xray_configs, \
                ic_electron_config=ic_electron_config, ic_xray_config=ic_xray_config, \
                proton_config=Br73_proton_config)

#add gammas for Se
with open(output_file, 'a') as f:
    f.write('\nGAMMA\n')
    f.write('72SE, %f, %f, 1\n'%(860+7096-4028,  7096-4028))


