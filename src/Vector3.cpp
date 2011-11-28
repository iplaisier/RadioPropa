// -*- C++ -*-
// $Id: ThreeVector.cc,v 1.3 2003/08/13 20:00:14 garren Exp $
// ---------------------------------------------------------------------------
//
// This file is a part of the CLHEP - a Class Library for High Energy Physics.
//
// This is the implementation of the Hep3Vector class.
//
// See also ThreeVectorR.cc for implementation of Hep3Vector methods which 
// would couple in all the HepRotation methods.
//

#ifdef GNUPRAGMA
#pragma implementation
#endif

#include "mpc/Vector3.h"
#include "mpc/PhysicalConstants.h"

#include <cmath>
#include <iostream>

namespace mpc {
void Vector3::setMag(double ma) {
  double factor = mag();
  if (factor == 0) {
    std::cerr << "Hep3Vector::setMag : zero vector can't be stretched" << std::endl;
  }else{
    factor = ma/factor;
    setX(x()*factor);
    setY(y()*factor);
    setZ(z()*factor);
  }
}

double Vector3::operator () (int i) const {
  switch(i) {
  case X:
    return x();
  case Y:
    return y();
  case Z:
    return z();
  default:
    std::cerr << "Hep3Vector subscripting: bad index (" << i << ")"
		 << std::endl;
  }
  return 0.;
}

double & Vector3::operator () (int i) {
  static double dummy;
  switch(i) {
  case X:
    return dx;
  case Y:
    return dy;
  case Z:
    return dz;
  default:
    std::cerr
      << "Hep3Vector subscripting: bad index (" << i << ")"
      << std::endl;
    return dummy;
  }
}

Vector3 & Vector3::rotateUz(const Vector3& NewUzVector) {
  // NewUzVector must be normalized !

  double u1 = NewUzVector.x();
  double u2 = NewUzVector.y();
  double u3 = NewUzVector.z();
  double up = u1*u1 + u2*u2;

  if (up>0) {
      up = sqrt(up);
      double px = dx,  py = dy,  pz = dz;
      dx = (u1*u3*px - u2*py)/up + u1*pz;
      dy = (u2*u3*px + u1*py)/up + u2*pz;
      dz =    -up*px +             u3*pz;
    }
  else if (u3 < 0.) { dx = -dx; dz = -dz; }      // phi=0  teta=pi
  else {};
  return *this;
}

double Vector3::pseudoRapidity() const {
  double m = mag();
  if ( m==  0   ) return  0.0;   
  if ( m==  z() ) return  1.0E72;
  if ( m== -z() ) return -1.0E72;
  return 0.5*log( (m+z())/(m-z()) );
}

std::ostream & operator<< (std::ostream & os, const Vector3 & v) {
  return os << "(" << v.x() << "," << v.y() << "," << v.z() << ")";
}

std::istream & operator>>(std::istream & is, Vector3 & v) {
  double x, y, z;
  is >> x;
  is >> y;
  is >> z;
  v.set(x, y, z);
  return  is;
}  // operator>>()

const Vector3 HepXHat(1.0, 0.0, 0.0);
const Vector3 HepYHat(0.0, 1.0, 0.0);
const Vector3 HepZHat(0.0, 0.0, 1.0);

//-------------------
//
// New methods introduced when ZOOM PhysicsVectors was merged in:
//
//-------------------

Vector3 & Vector3::rotateX (double phi) {
  double sinphi = sin(phi);
  double cosphi = cos(phi);
  double ty;
  ty = dy * cosphi - dz * sinphi;
  dz = dz * cosphi + dy * sinphi;
  dy = ty;
  return *this;
} /* rotateX */

Vector3 & Vector3::rotateY (double phi) {
  double sinphi = sin(phi);
  double cosphi = cos(phi);
  double tz;
  tz = dz * cosphi - dx * sinphi;
  dx = dx * cosphi + dz * sinphi;
  dz = tz;
  return *this;
} /* rotateY */

Vector3 & Vector3::rotateZ (double phi) {
  double sinphi = sin(phi);
  double cosphi = cos(phi);
  double tx;
  tx = dx * cosphi - dy * sinphi;
  dy = dy * cosphi + dx * sinphi;
  dx = tx;
  return *this;
} /* rotateZ */

bool Vector3::isNear(const Vector3 & v, double epsilon) const {
  double limit = dot(v)*epsilon*epsilon;
  return ( (*this - v).mag2() <= limit );
} /* isNear() */

double Vector3::howNear(const Vector3 & v ) const {
  // | V1 - V2 | **2  / V1 dot V2, up to 1
  double d   = (*this - v).mag2();
  double vdv = dot(v);
  if ( (vdv > 0) && (d < vdv)  ) {
    return sqrt (d/vdv);
  } else if ( (vdv == 0) && (d == 0) ) {
    return 0;
  } else {
    return 1;
  }
} /* howNear */

double Vector3::deltaPhi  (const Vector3 & v2) const {
  double dphi = v2.getPhi() - getPhi();
  if ( dphi > pi ) {
    dphi -= twopi;
  } else if ( dphi <= -pi ) {
    dphi += twopi;
  }
  return dphi;
} /* deltaPhi */

double Vector3::deltaR ( const Vector3 & v ) const {
  double a = eta() - v.eta();
  double b = deltaPhi(v); 
  return sqrt ( a*a + b*b );
} /* deltaR */

double Vector3::cosTheta(const Vector3 & q) const {
  double arg;
  double ptot2 = mag2()*q.mag2();
  if(ptot2 <= 0) {
    arg = 0.0;
  }else{
    arg = dot(q)/sqrt(ptot2);
    if(arg >  1.0) arg =  1.0;
    if(arg < -1.0) arg = -1.0;
  }
  return arg;
}

double Vector3::cos2Theta(const Vector3 & q) const {
  double arg;
  double ptot2 = mag2();
  double qtot2 = q.mag2();
  if ( ptot2 == 0 || qtot2 == 0 )  {
    arg = 1.0;
  }else{
    double pdq = dot(q);
    arg = (pdq/ptot2) * (pdq/qtot2);
        // More naive methods overflow on vectors which can be squared
        // but can't be raised to the 4th power.
    if(arg >  1.0) arg =  1.0;
 }
 return arg;
}

void Vector3::setEta (double eta) {
  double phi = 0;
  double r;
  if ( (dx == 0) && (dy == 0) ) {
    if (dz == 0) {
      std::cerr <<
        "Attempt to set eta of zero vector -- vector is unchanged" << std::endl;
      return;
    }
    std::cerr <<
      "Attempt to set eta of vector along Z axis -- will use phi = 0" << std::endl;
    r = fabs(dz);
  } else {
    r = getR();
    phi = getPhi();
  }
  double tanHalfTheta = exp ( -eta );
  double cosTheta =
        (1 - tanHalfTheta*tanHalfTheta) / (1 + tanHalfTheta*tanHalfTheta);
  dz = r * cosTheta;
  double rho = r*sqrt(1 - cosTheta*cosTheta);
  dy = rho * sin (phi);
  dx = rho * cos (phi);
  return;
}

void Vector3::setCylTheta (double theta) {

  // In cylindrical coords, set theta while keeping rho and phi fixed

  if ( (dx == 0) && (dy == 0) ) {
    if (dz == 0) {
    	std::cerr <<
        "Attempt to set cylTheta of zero vector -- vector is unchanged" << std::endl;
      return;
    }
    if (theta == 0) {
      dz = fabs(dz);
      return;
    }
    if (theta == pi) {
      dz = -fabs(dz);
      return;
    }
	std::cerr <<
      "Attempt set cylindrical theta of vector along Z axis "
      "to a non-trivial value, while keeping rho fixed -- "
      "will return zero vector" << std::endl;
    dz = 0;
    return;
  }
  if ( (theta < 0) || (theta > pi) ) {
	  std::cerr <<
      "Setting Cyl theta of a vector based on a value not in [0, PI]" << std::endl;
        // No special return needed if warning is ignored.
  }
  double phi (getPhi());
  double rho = getRho();
  if ( (theta == 0) || (theta == pi) ) {
	  std::cerr <<
      "Attempt to set cylindrical theta to 0 or PI "
      "while keeping rho fixed -- infinite Z will be computed" << std::endl;
      dz = (theta==0) ? 1.0E72 : -1.0E72;
    return;
  }
  dz = rho / tan (theta);
  dy = rho * sin (phi);
  dx = rho * cos (phi);

} /* setCylTheta */

void Vector3::setCylEta (double eta) {

  // In cylindrical coords, set eta while keeping rho and phi fixed

  double theta = 2 * atan ( exp (-eta) );

        //-| The remaining code is similar to setCylTheta,  The reason for
        //-| using a copy is so as to be able to change the messages in the
        //-| ZMthrows to say eta rather than theta.  Besides, we assumedly
        //-| need not check for theta of 0 or PI.

  if ( (dx == 0) && (dy == 0) ) {
    if (dz == 0) {
    	std::cerr <<
        "Attempt to set cylEta of zero vector -- vector is unchanged" << std::endl;
      return;
    }
    if (theta == 0) {
      dz = fabs(dz);
      return;
    }
    if (theta == pi) {
      dz = -fabs(dz);
      return;
    }
    std::cerr <<
      "Attempt set cylindrical eta of vector along Z axis "
      "to a non-trivial value, while keeping rho fixed -- "
      "will return zero vector" << std::endl;
    dz = 0;
    return;
  }
  double phi (getPhi());
  double rho = getRho();
  dz = rho / tan (theta);
  dy = rho * sin (phi);
  dx = rho * cos (phi);

} /* setCylEta */


Vector3 operator/  ( const Vector3 & v1, double c ) {
  if (c == 0) {
	  std::cerr <<
      "Attempt to divide vector by 0 -- "
      "will produce infinities and/or NANs" << std::endl;
  } 
  double   oneOverC = 1.0/c;
  return Vector3  (  v1.x() * oneOverC,
                        v1.y() * oneOverC,
                        v1.z() * oneOverC );
} /* v / c */

Vector3 & Vector3::operator/= (double c) {
  if (c == 0) {
	  std::cerr <<
      "Attempt to do vector /= 0 -- "
      "division by zero would produce infinite or NAN components" << std::endl;
  }
  double oneOverC = 1.0/c;
  dx *= oneOverC;
  dy *= oneOverC;
  dz *= oneOverC;
  return *this;
}

double Vector3::tolerance = Vector3::ToleranceTicks * 2.22045e-16;

} // namespace mpc