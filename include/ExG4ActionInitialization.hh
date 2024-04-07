#ifndef ExG4ActionInitialization_h
#define ExG4ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include "G4Types.hh"

class ExG4ActionInitialization : public G4VUserActionInitialization
{
public:
   ExG4ActionInitialization();
   virtual ~ExG4ActionInitialization();

   virtual void BuildForMaster() const;
   virtual void Build() const;


  G4double m_Erecoil;

};

#endif
