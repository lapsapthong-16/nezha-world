#ifndef MODEL_HPP
#define MODEL_HPP

// global proportions (same as your struct)
struct ModelScale {
    float torsoH = 1.02f;
    float torsoTopR = 0.70f;
    float torsoBotR = 0.82f;

    float headLift = 0.92f;
    float headR = 0.95f;

    float shoulderX = 0.92f;
    float shoulderY = 0.66f;

    float upperArmR = 0.26f;
    float upperArmH = 0.56f;
    float lowerArmR = 0.24f;
    float lowerArmH = 0.48f;
    float jointR = 0.18f;

    float shortSleeveDrop = 0.86f;
    float shortSleeveLen = 0.44f;

    float hipX = 0.34f;
    float upperLegH = 0.40f;
    float lowerLegH = 0.42f;
};

// single global instance (declared here, defined in model.cpp)
extern ModelScale MS;

#endif
