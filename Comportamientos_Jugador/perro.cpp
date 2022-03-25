#include "../Comportamientos_Jugador/perro.hpp"
#include "motorlib/util.h"


#include <iostream>
#include <stdlib.h>


Action ComportamientoPerro::think(Sensores sensores){

  Action accion;

  if (sensores.superficie[2] == 'j'){
    accion = actWHEREIS;
  }
  else if (sensores.terreno[2] != 'P' and sensores.terreno[2] != 'M' and sensores.superficie[2] != '_'){
          if (sensores.superficie[6] == 'j' or sensores.superficie[12] == 'j'){
            accion = actFORWARD;
          }
          else if (sensores.superficie[10] == 'j' or sensores.superficie[11] == 'j' or sensores.superficie[13] == 'j' or sensores.superficie[14] == 'j'){
            accion = actFORWARD;
          }
  }
  else if (sensores.superficie[1] == 'j' or sensores.superficie[4] == 'j' or sensores.superficie[9] == 'j'){
    accion = actSEMITURN_L;
  }
  else if (sensores.superficie[3] == 'j' or sensores.superficie[8] == 'j' or sensores.superficie[15] == 'j'){
    accion = actSEMITURN_R;
  }
  else {
    int cual = aleatorio(20);
    switch (cual) {
    case 0: accion = actIDLE; break;
    case 1: accion = actTURN_L; break;
    case 2: accion = actTURN_R; break;
    case 3: accion = actSEMITURN_L; break;
    case 4: accion = actSEMITURN_R; break;
    default: 
      if (sensores.terreno[2] != 'P' and sensores.terreno[2] != 'M' and sensores.superficie[2] == '_') accion = actFORWARD;
      else accion = actSEMITURN_L;
      break;
    }
  }

  return accion;
}

int ComportamientoPerro::interact(Action accion, int valor){
  return 0;
}
