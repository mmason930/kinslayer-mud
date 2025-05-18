#include "../conf.h"

#include "JSZone.h"
#include "js_interpreter.h"

void JSEnvironment::LoadJSZone()
{
    load_class<JSZone>();
}

void JSZone::echo(flusspferd::string message)
{
    if(real)
        sendToZone(flusspferd::string::concat(message, "\r\n").c_str(), real->GetRnum());
}

void JSZone::reset()
{
    if(real)
        real->Reset();
}

int JSZone::distanceTo(JSZone *other)
{
    if(real && other && other->toReal())
        return real->Distance(other->toReal());
    return -1;
}
