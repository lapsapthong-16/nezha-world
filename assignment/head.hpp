#ifndef HEAD_HPP
#define HEAD_HPP

// Head spinning state
extern float gHeadSpinAngle; // Y-axis rotation angle for head spinning

// Head spinning functions
void spinHeadLeft();
void spinHeadRight();
void resetHeadSpin();
void drawHeadUnit();

#endif