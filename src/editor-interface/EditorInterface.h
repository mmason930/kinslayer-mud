#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <list>
#include <initializer_list>
#include <boost/optional.hpp>
#include <functional>

typedef std::function<boost::optional<std::string>(const std::string &)> ValidationFunction;

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../screen.h"
#include "../utils.h"
#include "EditorInterfaceInstance.h"
#include "EditorInterfaceMenu.h"
#include "EditorInterfaceData.h"

#include "EditorInterfaceBase.h"

template <typename DataType>
class EditorInterface : public EditorInterfaceBase
{
private:
protected:
public:

	DataType *getData(EditorInterfaceInstance *i)
	{
		return (DataType*)i->getData();
	}

	EditorInterfaceData *createData() const
	{
		return new DataType();
	}

	EditorInterface() : EditorInterfaceBase() {}
};

#endif