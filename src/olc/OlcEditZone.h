#ifndef OLC_EDIT_ZONE_H
#define OLC_EDIT_ZONE_H

#include "OlcEdit.h"

class OlcEditZone : public OlcEdit
{
private:
	int zoneId;
public:

	int getZoneId() const;
	{
		return zoneId;
	}

	void setZoneId(int zoneId)
	{
		this->zoneId = zoneId;
	}
}

#endif
