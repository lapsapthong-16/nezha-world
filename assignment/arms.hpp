#ifndef ARMS_HPP
#define ARMS_HPP

// ===== Per-arm lift state (abduction) =====
extern float gLeftArmLiftAngle;   // Left arm lift angle
extern float gRightArmLiftAngle;  // Right arm lift angle

// ===== Per-arm elbow bend state =====
extern float gLeftElbowBendAngle;   // Left elbow bend angle (side bend, Z axis)
extern float gRightElbowBendAngle;  // Right elbow bend angle (up/down, X axis)

// ===== Lift controls =====
void liftLeftArm();
void liftRightArm();
void lowerLeftArm();
void lowerRightArm();
void resetLeftArm();
void resetRightArm();
void resetBothArms();

// ===== Elbow bend controls =====
void bendLeftElbow();
void straightenLeftElbow();
void bendRightElbow();
void straightenRightElbow();
void resetLeftElbow();
void resetRightElbow();
void resetBothElbows();

// ===== Draw =====
void drawArmChain(bool left);

#endif // ARMS_HPP
