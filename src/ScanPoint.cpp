#include "ScanPoint.hpp"

ScanPoint::ScanPoint(){
  fX=0.; fY=0.; fZ=0.;
  fEntry=0;
  fEntries=0; // haven't set an entry yet
}

ScanPoint::ScanPoint( double x, double y, double z, unsigned long long entry ){
  fX = x;  fY = y;  fZ = z;
  fEntry = entry;
  fEntries = 0;
}
