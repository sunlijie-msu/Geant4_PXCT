'''
input_name, output_name: name of the input and output files
ic_eject_fraction, ec_capture_fraction: capture fractions for internal conversion and EC electrons
write_mode: Change to 'a' to append generated content to end of the file.

Input file is assumed to be in ENSDF format, and the output file will be in the format expected by 
my GEANT4 decay scheme. Partial lifetimes are set to 1/branching ratio, which should give the correct
branching ratios.

For ENSDF format specification, see http://nt.web.nitech.ac.jp/softwares/isoexpl/ensdfman.pdf

 == Decay Scheme File Format Specification == 
  All characters following a # on a given
  line will be ignored. The file will consist of a arbitrary number of
  state transition tables. To start a table, write a single line with
  the table type. All energies should be in keV.

  GAMMA
  isotope name, initial energy in keV, final energy in keV, partial lifetime in seconds

  ELECTRON_CAPTURE
  initial isotope name, initial isotope energy, final isotope name, final energy, partial lifetime in second, e- frac 1, e- frac 2, ...

  BETA_PLUS or PROTON or ALPHA
  initial isotope name, initial isotope energy, final isotope name, final isotope energy, partial lifetime in seconds

  INTERNAL_CONVERSION
  isotope name, initial energy, final energy, e- frac 1, e- frac 2, ...

  The probabilities for vacancies created by electron capture or internal conversion should be ordered as: K1, L1, L2, L3, M1, ...
'''
def convert_file(input_name, output_name, ec_capture_fraction=[1.], \
                 ic_eject_fraction=[1.], write_mode='w'):
    cards = get_cards(input_name) #cards are what the ENSDF file format calls a line of text
    levels = get_levels(cards)
    #will store a list of all of each type of transition, so that they can be 
    #written together at the end
    gammas = []
    electron_captures = []
    beta_plus = []
    internal_conversions = [] #will be calculated based on conversion coefficient
    #parse cards
    isotope = ''
    energy = -1 #records proceeding first level haven't been placed and shouldn't be included
    ec_parent_decay_energy = 0 #will be recorded when bp to ground decay record is read in
    for card in cards:
        isotope = card[0:5].strip()
        record_type = card[5:9].strip()
        if record_type.upper() == 'L': #level record
            energy = float(card[9:19])
        elif record_type.upper() == 'PN': #normalization record
            assert True#card[77] == '6'
        elif record_type.upper() == 'G':
            if energy == -1:
                continue
            decay_energy = float(card[9:19])
            ri_string = card[21:29]
            #don't store it if it doesn't have a relative intensity associated with it
            if(len(ri_string.strip()) != 0): 
                photon_intensity = float(ri_string)
                E_init = get_closest_level(levels, isotope, energy)['energy']
                E_final = get_closest_level(levels, isotope, energy - decay_energy)['energy']
                cc_string = card[55:62].strip()
                conversion_coef = float(cc_string) if len(cc_string)>0 else 0.
                gammas.append({'isotope':isotope, 'E_i':E_init, 
                               'E_f':E_final, 
                               'weight':photon_intensity})
                internal_conversions.append({'isotope': isotope, 'E_i':E_init,
                           'E_f':E_final, 'weight':photon_intensity*conversion_coef})
        elif record_type == 'E': #electron capture/beta+
            ec_weight, bp_weight = card[31:39].strip(), card[21:29].strip()
            ec_weight = float(ec_weight) if len(ec_weight) > 0 else 0
            bp_weight = float(bp_weight) if len(bp_weight) > 0 else 0
            if ec_weight == 0 and bp_weight == 0: #don't store it if it never happens
                continue
            final_energy = get_closest_level(levels, isotope, energy)['energy']
            electron_captures.append({'X_i':ec_parent,
                                      'E_i':ec_parent_energy,
                                      'X_f':isotope,
                                      'E_f':final_energy,
                                      'weight':ec_weight,
                                     })
            beta_plus.append({'X_i':ec_parent,
                                      'E_i':ec_parent_energy,
                                      'X_f':isotope,
                                      'E_f':final_energy,
                                      'weight':bp_weight})
        elif record_type == 'P': #parent record
            ec_parent_energy = float(card[64:74].strip())
            ec_parent = isotope

    #write output file
    with open(output_name, write_mode) as output_file:
        output_file.write('GAMMA\n')
        for gamma in gammas:
            output_file.write(gamma['isotope'] + ', ' + str(gamma['E_i']) + ', ' \
                + str(gamma['E_f']) + ', ' + str(1/gamma['weight']) + '\n')  
        output_file.write('\n')

        output_file.write('INTERNAL_CONVERSION\n')
        
        for ic in internal_conversions:
            if(ic['weight'] == 0):
                continue #don't include, would lead to divide by 0
            output_file.write(ic['isotope'] + ', ' + str(ic['E_i']) + ', '\
                              + str(ic['E_f']) + ', ' + str(1/ic['weight']))
            for fp in ic_eject_fraction:
                output_file.write(', ' + str(fp))
            output_file.write('\n')
        
        output_file.write('\nELECTRON_CAPTURE\n')
        for ec in electron_captures:
            if ec['weight']==0:
                continue #don't include, would lead to divide by 0
            output_file.write(ec['X_i'] + ', ' + str(ec['E_i']) + ', ' + ec['X_f'] \
                               + ', ' + str(ec['E_f']) + ', ' + str(1/ec['weight']))
            for fp in ec_capture_fraction:
                output_file.write(', ' + str(fp))
            output_file.write('\n')
        
        output_file.write('BETA_PLUS\n')
        for bp in beta_plus:
            if bp['weight']==0:
                continue #don't include, would lead to divide by 0
            output_file.write(bp['X_i'] + ', ' + str(bp['E_i']) + ', ' + bp['X_f'] \
                               + ', ' + str(bp['E_f']) + ', ' + str(1/bp['weight']))
            output_file.write('\n')

    
            
def get_cards(input_name):
    with open(input_name, 'r') as input_file:  
        cards = input_file.readlines()
    return cards

def get_levels(cards):
    '''
    returns list of dicts which have format (isotope, energy, energy uncertainty)
    '''
    levels = []
    for card in cards:
        record_type = card[5:9].strip()
        if record_type.upper() == 'L':
            isotope = card[0:5].strip()
            energy = float(card[9:19])
            levels.append({'isotope':isotope, 'energy':energy})
    return levels

def get_closest_level(levels, isotope, energy):
    to_return = None
    for level in levels:
        if level['isotope'] != isotope:
            continue
        if to_return == None:
            to_return = level
        else:
            if abs(energy - level['energy']) < abs(energy - to_return['energy']):
                to_return = level
    if(abs(energy - to_return['energy']) >= max(2, energy*0.01)):
        print(energy, to_return['energy'])
    assert abs(energy - to_return['energy']) < max(2, energy*0.01) #would be weird if it was more off than this
    return to_return

class decay_scheme:
    def __init__(self):
        Qs = {} #dictionary of Q values between ground states, indexed by (parent name, daughter name)

class isotope:
    def __init__(self, name):
        this.name = name
        parents = {} #dictionary indexed by parent name, containing relative Q values
        daughters = {} 
