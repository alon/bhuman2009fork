
#ifndef ALMODULEINFOR_H
#define ALMODULEINFOR_H

namespace AL
{


typedef std::string	xsd__string; // encode xsd__string value as the xsd:string schema type 
typedef int xsd__int; // encode xsd__int value as the xsd:int schema type
typedef bool xsd__bool;			// Alma 08-01-21: WARNING: this value will have a 8 bits size ! le top aurait etait d'avoir un bool sur 32 bits, mais ca pose des problemes au compilo pour diff?rencier les operateurs qui renvoient du bool de ceux du int
typedef double xsd__double;
typedef float xsd__float;



class al__ALModuleInfoNaoqi
{
public:
 	xsd__string name; 
	xsd__int architecture; 
	xsd__string ip;
	xsd__int port;
	xsd__int processId;
	xsd__int modulePointer;
	xsd__bool isABroker;
	xsd__bool keepAlive;
};




}

#endif


