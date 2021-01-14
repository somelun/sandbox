#pragma once

#include <vector>
#include "../maths.h"

struct RoadSegment {
    uint16_t n;
    vector3i worldPoint1;
    vector3i screenPoint1;
    vector3i cameraPoint1;
    vector3i worldPoint2;
    vector3i screenPoint2;
    vector3i cameraPoint2;
    Color color;
};

class RaceScene {
public:
    //FIXME: remove framebuffer from here
    RaceScene(class Framebuffer& buffer);

    void update(double dt);

private:
    class Framebuffer& buffer_;

    float distance_{0.0f};
    float length_{0.0f};

    uint16_t width_{640};
    uint16_t height_{480};

//    std::vector<std::pair<float, float>> track_data_;

    void update2(double dt);

    std::vector<RoadSegment*> road_;
    
    RoadSegment* findSegment();
    
//    void Project(vector3i& point, vector3i cameraCoords);
    
};
