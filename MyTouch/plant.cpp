#include "plant.h"

void DocumentParser::Analysis() {
	if (!doc.children("config").empty()) {
		pugi::xml_node mouse = doc.child("config").child("mouse");
		speed = mouse.attribute("speed").as_float(2);
	}
	IDocumentParser::Analysis();
}