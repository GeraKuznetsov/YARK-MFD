#include "Grapher.h"
#include "Util/IM.h"
#include "../Log.h"

Grapher::Grapher(int i) {
	statI = i;
}

std::string Grapher::GetTitle() {
	switch (statI) {
	case 0:
		return "G Force Graph";
	case 1:
		return "Altitude Graph";
	case 2:
		return "Surface Velocity Graph";
	case 3:
		return "Orbital Velocity Graph";
	case 4:
		return "CurrentStage Graph";
	}
	return "Graph";
}

#define PADDING_Y_START 15
#define PADDING_Y_END 35
#define PADDING_X_START 80
#define PADDING_X_END 15

void Grapher::Draw(XY pos, XY size) {
	float maxim = 0;
	for (int i = FirstLog; i < LOG_NUM_ENTRIES; i++) {
		if (logs[i].logData[statI] > maxim) maxim = logs[i].logData[statI];
	}
	//maxim = 759;
	float graphHeight = (size.y - PADDING_Y_START - PADDING_Y_END);
	int curHeight = PADDING_Y_START;
	draw->SwitchShader(SHADER_2D);
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0.2, 0.2, 0.2);
	draw->DrawRect2D(pos.x + PADDING_X_START, pos.y + curHeight, pos.x + size.x - PADDING_X_END, pos.y + curHeight + graphHeight);
	draw->SetDrawColor2D(1, 1, 1);

	float graphTime = LOG_NUM_ENTRIES;
	int barXTime = 30;
	float graphWidth = float(size.x - PADDING_X_START - PADDING_X_END);
	int numBars = LOG_NUM_ENTRIES / barXTime;
	float barDelta = graphWidth / float(numBars);
	int timeMult = 1;
	while (barDelta < 50) {
		numBars /= 2;
		timeMult *= 2;
		barDelta = graphWidth / float(numBars);
	}
	float startBarX = pos.x + PADDING_X_START;
	draw->SetDrawColor2D(1, 1, 1);
	for (int i = 0; i < numBars + 1; i++) {
		draw->DrawRect2D(startBarX, pos.y + PADDING_Y_START, startBarX + 1, pos.y + size.y - PADDING_Y_END);
		startBarX += barDelta;
	}

	draw->DrawRect2D(pos.x + PADDING_X_START, pos.y + PADDING_Y_START, pos.x + size.x - PADDING_X_END, pos.y + PADDING_Y_START + 1);
	int maxMult = 1;
	while (maxim / maxMult >= 10) {
		maxMult *= 10;
	}
	for (int i = 0; i < maxim; i += maxMult) {
		int y = graphHeight - (i / maxim * graphHeight);
		draw->DrawRect2D(pos.x + PADDING_X_START, pos.y + PADDING_Y_START + y, pos.x + size.x - PADDING_X_END, pos.y + PADDING_Y_START + y + 1);
	}


	draw->SwitchShader(SHADER_TEXT);
	{
		std::string text = std::to_string(int(maxim));
		draw->DrawString(f, text, pos.x + PADDING_X_START - f->GetTextWidth(text) - 20, pos.y + PADDING_Y_START + 10);
	}
	for (int i = 0; i < maxim; i += maxMult) {
		int y = graphHeight - (i / maxim * graphHeight);
		std::string text = std::to_string(i);
		draw->DrawString(f, text, pos.x + PADDING_X_START - f->GetTextWidth(text) - 20, pos.y + PADDING_Y_START + y + 10);
	}

	startBarX = pos.x + PADDING_X_START;
	for (int i = 0; i < numBars + 1; i++) {
		std::string text = std::to_string(i * barXTime * timeMult);
		draw->DrawString(f, text, startBarX - f->GetTextWidth(text, -1, 0.9f) / 2, pos.y + size.y - PADDING_Y_END + 20, 0.9f);
		startBarX += barDelta;
	}

	draw->SetDrawColor2D(1, 0, 0);
	glLineWidth(2);
	float startTime = logs[LOG_NUM_ENTRIES - 1].time - graphTime;
	for (int i = FirstLog + 1; i < LOG_NUM_ENTRIES; i++) {
		int x = (logs[i].time - startTime) / graphTime * graphWidth;
		int y = logs[i].logData[statI] / maxim * graphHeight;

		int x1 = (logs[i - 1].time - startTime) / graphTime * graphWidth;
		int y1 = logs[i - 1].logData[statI] / maxim * graphHeight;

		y = (graphHeight - y);
		y1 = (graphHeight - y1);

		if (x > 0 && x1 > 0) {
			draw->DrawLine2D(pos.x + PADDING_X_START + x, pos.y + PADDING_Y_START + y, pos.x + PADDING_X_START + x1, pos.y + PADDING_Y_START + y1);
		}
	}
}

