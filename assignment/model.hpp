#ifndef MODEL_HPP
#define MODEL_HPP

// Global character proportions
struct ModelScale {
    // Torso dimensions
    float torsoH = 1.18f;
    float torsoTopR = 0.62f;
    float torsoBotR = 0.74f;

    // Head positioning
    float headLift = 0.92f;
    float headR = 0.95f;

    // Shoulder positioning
    float shoulderX = 0.84f;
    float shoulderY = 0.74f;

    // Arm dimensions
    float upperArmR = 0.22f;
    float upperArmH = 0.50f;
    float lowerArmR = 0.20f;
    float lowerArmH = 0.44f;
    float jointR = 0.16f;

    // Leg and hip dimensions
    float shortSleeveDrop = 0.88f;
    float shortSleeveLen = 0.38f;
    float hipX = 0.30f;
    float upperLegH = 0.38f;
    float lowerLegH = 0.40f;
};

// Global model scale instance
extern ModelScale MS;

#endif
