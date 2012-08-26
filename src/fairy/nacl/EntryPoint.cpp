#include <iostream>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/instance.h>
#include "CycloaInstance.h"

class CycloaModule : public pp::Module {
public:
	CycloaModule() : pp::Module() {}
	virtual ~CycloaModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new CycloaInstance(this->core(), instance);
	}
};

namespace pp {

Module* CreateModule() {
	return new CycloaModule();
}

}
