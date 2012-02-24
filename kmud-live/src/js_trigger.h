

#ifdef KINSLAYER_JAVASCRIPT

#ifndef KINSLAYER_JS_TRIGGER_H
#define KINSLAYER_JS_TRIGGER_H

#include <string>
#include <sstream>
#include <iostream>
#include "utils.h"

using namespace std;

struct JSTrigger
{
    int vnum;
    bool code_modified;
    bool valid; // can be validly run
	bool running; // Is this trigger running?
	bool deleted; // Is this trigger set to delete over reboot?
    std::string name; // name displayed to scripters
    short narg; // numerical arg (used by some trigger types)
    std::string args; // string args (used by some trigger types)
    std::string text; // text of the script
    unsigned long long trigger_flags; // type of trigger (greet, etc)
    unsigned long allowed_flags;
	unsigned long option_flags;

    bool isFlagged(int _flag) { return trigger_flags & Q_BIT(_flag); }
    void setFlag(int _flag) { trigger_flags |= Q_BIT(_flag); }
	void toggleFlag(int _flag) { TOGGLE_BIT(trigger_flags, Q_BIT(_flag)); }

	bool optionFlagged( int _flag ) { return option_flags & Q_BIT(_flag); }
	void setOptionFlag( int _flag ) { option_flags |= Q_BIT(_flag); }
	void toggleOptionFlag( int _flag ) { TOGGLE_BIT(option_flags, Q_BIT(_flag)); }

    bool isAllowed(int _flag) { return allowed_flags & Q_BIT(_flag); }
    void setAllowed(int _flag) { allowed_flags |= Q_BIT(_flag); }
	void toggleAllowed(int _flag) { TOGGLE_BIT(allowed_flags, Q_BIT(_flag)); }

    std::string js_name; // internal name used by javascript

    JSTrigger(int _vnum)
        : vnum(_vnum), code_modified(false), valid(false), deleted(false) // we need to compile it before its valid.
    {
		narg=trigger_flags=allowed_flags=0;
        stringstream s;
        s << "trig" << vnum;
        js_name = s.str();
		option_flags = 0;
	}

	JSTrigger* clone()
	{
		JSTrigger * t = new JSTrigger(vnum);
		t->text = text; // deep copy
		t->args = args;
		t->trigger_flags = trigger_flags;
		t->allowed_flags = allowed_flags;
		t->code_modified = false;
		t->name = name;
		t->deleted = deleted;
		t->narg = narg;
        t->js_name = js_name;
        t->valid = valid;
		t->running = running;
		t->option_flags = option_flags;
		return t;
	}

    JSTrigger& operator=(JSTrigger& t)
    {
        text = t.text; // deep copy
        args = t.args;
        valid = t.valid;
        trigger_flags = t.trigger_flags;
        allowed_flags = t.allowed_flags;
        code_modified = t.code_modified;
        name = t.name;
		deleted=t.deleted;
        narg = t.narg;
        js_name = t.js_name;
		option_flags = t.option_flags;
        return *this;
    }
};

#endif
#endif

