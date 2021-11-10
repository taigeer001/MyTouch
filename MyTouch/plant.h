#pragma once

#include <vector>
#include <map>
#include "module.h"


class DocumentParser :public IDocumentParser {
public:
	void Analysis() override;
};
