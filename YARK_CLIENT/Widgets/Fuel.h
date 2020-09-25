#include "Widget.h"

class FuelDisplay : public Widget {
public:
	FuelDisplay();
	std::string GetTitle();
	void Draw(XY pos, XY size);
};