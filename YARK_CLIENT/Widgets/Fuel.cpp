#include "Fuel.h"
#include <glm/vec3.hpp>

std::string FuelDisplay::GetTitle() {
	return "Fuel levels";
}

FuelDisplay::FuelDisplay() {

}

#define PADDING 30
#define BAR_WIDTH 70
#define PADDING_Y_START 15
#define PADDING_Y_END 50

void DrawBar(int x, XY pos, XY size, std::string fuel, glm::vec3 color, float full, float have, float haveStage) {
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(0.2, 0.2, 0.2);
	draw->DrawRect2D(x, pos.y + PADDING_Y_START, x + BAR_WIDTH, pos.y + size.y - PADDING_Y_END);
	float height = size.y - PADDING_Y_END - PADDING_Y_START;
	float y = (pos.y + size.y - PADDING_Y_END) - (have / full * height);
	float y2 = y;
	if (haveStage != -1) {
		y2 = y +  (haveStage / full * height);
	}
	std::string percent = std::to_string(int(have / full * 100)) + "%";
	draw->SetDrawColor2D(color.x, color.y, color.z);
	draw->DrawRect2D(x, y, x + BAR_WIDTH, y2);
	if (haveStage != -1) color *= 0.5f;
	draw->SetDrawColor2D(color.x, color.y, color.z);
	draw->DrawRect2D(x, y2, x + BAR_WIDTH, pos.y + size.y - PADDING_Y_END);
	draw->SetDrawColor2D(1, 0, 0);
	draw->DrawRect2D(x, pos.y + PADDING_Y_START, x + 2, pos.y + size.y - PADDING_Y_END);
	draw->DrawRect2D(x + BAR_WIDTH, pos.y + PADDING_Y_START, x + BAR_WIDTH - 2, pos.y + size.y - PADDING_Y_END);
	draw->DrawRect2D(x, pos.y + PADDING_Y_START, x + BAR_WIDTH, pos.y + PADDING_Y_START + 2);
	draw->DrawRect2D(x, pos.y + size.y - PADDING_Y_END - 2, x + BAR_WIDTH, pos.y + size.y - PADDING_Y_END);

	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, fuel, x + (BAR_WIDTH - f->GetTextWidth(fuel)) / 2, pos.y + size.y - PADDING_Y_END + 25);
	draw->DrawString(f, percent, x + (BAR_WIDTH - f->GetTextWidth(percent)) / 2, y + 25);
}

void FuelDisplay::Draw(XY pos, XY size) {
	float spaceWidth = float(size.x - PADDING - PADDING - 6 * BAR_WIDTH) / 5.f;
	float x = pos.x + PADDING;
	draw->SwitchShader(SHADER_2D);
		auto CV = client.Vessel;
	DrawBar(x, pos, size, "LiquidFuel", glm::vec3(0.13, 0.2, 1), CV.LiquidFuelTot, CV.LiquidFuel, CV.LiquidFuelS); x += BAR_WIDTH + spaceWidth;
	DrawBar(x, pos, size, "Oxidizer", glm::vec3(0.8, 0.1, 0.1), CV.OxidizerTot, CV.Oxidizer, CV.OxidizerS); x += BAR_WIDTH + spaceWidth;
	DrawBar(x, pos, size, "SolidFuel", glm::vec3(0.1, 1, 0.1), CV.SolidFuelTot, CV.SolidFuel, -1); x += BAR_WIDTH + spaceWidth;
	DrawBar(x, pos, size, "ECharge", glm::vec3(0.9, 0.9, 0.1), CV.EChargeTot, CV.ECharge, -1); x += BAR_WIDTH + spaceWidth;
	DrawBar(x, pos, size, "MonoProp", glm::vec3(0.92, 0.79, 1), CV.MonoPropTot, CV.MonoProp, -1); x += BAR_WIDTH + spaceWidth;
	DrawBar(x, pos, size, "XenonGas", glm::vec3(0.54, 0.02, 0.99), CV.XenonGasTot, CV.XenonGas, -1); x += BAR_WIDTH + spaceWidth;
}