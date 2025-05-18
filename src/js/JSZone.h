#ifndef KINSLAYER_JSZONE_H
#define KINSLAYER_JSZONE_H

#include <flusspferd.hpp>
#include <string>

#include "../zones.h"
#include "js_utils.h"
#include "JSRoom.h" // for sendToZone declaration

using namespace flusspferd;
using namespace std;

FLUSSPFERD_CLASS_DESCRIPTION(
    JSZone,
    (full_name, "JSZone")
    (constructor_name, "JSZone")
    (methods,
        ("echo", bind, echo)
        ("reset", bind, reset)
        ("distanceTo", bind, distanceTo)
    )
    (properties,
        ("name", getter, getName)
        ("vnum", getter, getVnum)
        ("bottom", getter, getBottom)
        ("top", getter, getTop)
        ("lifespan", getter, getLifespan)
        ("age", getter, getAge)
        ("closed", getter, getClosed)
        ("x", getter, getX)
        ("y", getter, getY)
    ))
{
public:
    JSZone(flusspferd::object const &self, flusspferd::call_context& cc)
        : base_type(self) {}
    JSZone(flusspferd::object const &self, Zone *real)
        : base_type(self) { this->real = real; }
    ~JSZone() {}

    Zone *toReal() { return real; }
    void setReal(Zone *z) { real = z; }

    flusspferd::string getName() { return real ? real->getName() : ""; }
    int getVnum() { return real ? real->getVnum() : -1; }
    int getBottom() { return real ? real->GetBottom() : 0; }
    int getTop() { return real ? real->GetTop() : 0; }
    int getLifespan() { return real ? real->GetLifespan() : 0; }
    int getAge() { return real ? real->GetAge() : 0; }
    bool getClosed() { return real ? real->IsClosed() : true; }
    int getX() { return real ? real->GetX() : 0; }
    int getY() { return real ? real->GetY() : 0; }

    void echo(flusspferd::string message);
    void reset();
    int distanceTo(JSZone *other);

private:
    Zone *real;
};

#endif
