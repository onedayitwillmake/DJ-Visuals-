/*
 * ConstraintType.h
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	// TODO: FILL IN IDEA OF CLASS
 */


#ifndef CONSTRAINTTYPE_H_
#define CONSTRAINTTYPE_H_

namespace oneday { namespace steering {

}
// enumerate demos of various constraints on the flock
enum ConstraintType {
	none,
	insideSphere,
	outsideSphere,
	outsideSpheres,
	outsideSpheresNoBig,
	rectangle,
	rectangleNoBig,
	outsideBox,
	insideBox
};
//ConstraintType constraint;
} /* namespace steering */
} /* namespace oneday */
#endif /* CONSTRAINTTYPE_H_ */
