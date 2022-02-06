#ifndef KINSLAYER_JS_TRIGGER_H
#define KINSLAYER_JS_TRIGGER_H

#include <string>
#include <sstream>
#include <iostream>
#include "../utils.h"

using namespace std;

struct JSTrigger
{
	//std::string text; // text of the script
    //std::string js_name; // internal name used by javascript

    int vnum;
	int scriptId;
    bool code_modified;
	bool running; // Is this trigger running?
	bool deleted; // Is this trigger set to delete over reboot?
    short narg; // numerical arg (used by some trigger types)
    std::string name; // name displayed to scripters
    std::string args; // string args (used by some trigger types)
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

    JSTrigger(int _vnum)
        : vnum(_vnum), code_modified(false), deleted(false)
    {
		
		option_flags = narg = trigger_flags = allowed_flags = 0;
		
		scriptId = -1;
	}

	JSTrigger* clone()
	{
		JSTrigger * t = new JSTrigger(vnum);
		t->scriptId = scriptId;
		t->args = args;
		t->trigger_flags = trigger_flags;
		t->allowed_flags = allowed_flags;
		t->code_modified = false;
		t->name = name;
		t->deleted = deleted;
		t->narg = narg;
		t->running = running;
		t->option_flags = option_flags;
		return t;
	}

    JSTrigger& operator=(JSTrigger& t)
    {
		scriptId = t.scriptId;
        args = t.args;
        trigger_flags = t.trigger_flags;
        allowed_flags = t.allowed_flags;
        code_modified = t.code_modified;
        name = t.name;
		deleted=t.deleted;
        narg = t.narg;
		option_flags = t.option_flags;
        return *this;
    }
};

#endif
