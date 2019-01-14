#include <stdio.h>
#include <iostream>
#include "simplesignalslot.h"
using namespace simplesignalslot;
//#include "sigslot.h"
using namespace std;
//using namespace sigslot;

class Switch
{
	public:
	signal clicked;
};

class Light : public has_slots
{
	public:
	void turn_on()
	{
		std::cout<<"Turn on ~"<<std::endl;
	}
};

int main(void)
{
	Light lit;
	Switch swh;
	
	swh.clicked.connect(&lit,&Light::turn_on);
	swh.clicked.emit();
	return 0;
}