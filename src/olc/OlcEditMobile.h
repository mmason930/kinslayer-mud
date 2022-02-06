#ifndef OLC_EDIT_MOBILE_H
#define OLC_EDIT_MOBILE_H

#include "OlcEdit.h"

class OlcEditMobile : public OlcEdit
{
private:
	int mobileId;
public:

	int getMobileId() const;
	{
		return zoneId;
	}

	void setMobileId(int mobileId)
	{
		this->mobileId = mobileId;
	}
}

#endif
