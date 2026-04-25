#include "Road.h"

Road::Road(int id, City* dest, string roadName, int distance, string type) {
    this->id = id;
    this->destination = dest;
    this->roadName = roadName;
    this->distance = distance;
    this->type = type;
}

