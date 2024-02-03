#include <string>
#include <thread> // Include for std::thread
#include <chrono> // Include for std::chrono
#include "BatObfuscate.h"
#include "Drawing.h"
#include "encrypt.h"
#include "Settings.h"
#include "ui.h"
#include "Windows.h"

ImVec2 Drawing::vWindowSize = { 340, 109 };
ImGuiWindowFlags Drawing::WindowFlags = ImGuiWindowFlags_NoDecoration;
bool Drawing::bDraw = true;
std::string filePathBuffer;
bool isObfuscating = false;

static bool statusDisplayed = false;

void errorBeep() {
	// Call Beep function
	Beep(100, 500);
}

void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

void drawUpperRectAndLine() {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	drawList->AddRectFilled(ImVec2(windowPos.x + 1, windowPos.y), ImVec2(windowPos.x + windowSize.x, windowPos.y + 25), ImColor(30, 30, 30, 255)); // upper rect
	drawList->AddLine(ImVec2(windowPos.x + 1, windowPos.y + 25), ImVec2(windowPos.x + windowSize.x, windowPos.y + 25), ImColor(255, 255, 255, 15)); // upper line
}

void Drawing::WindowHandle() {
	ImGui::PushFont(UI::mainfont);
	drawUpperRectAndLine();
	ImVec2 closeButtonPos = ImVec2(ImGui::GetWindowSize().x - 17, 5);
	ImGui::SetCursorPos(ImVec2(5, 6));
	ImGui::Text(x("Bat Obfuscator"));
	ImGui::SetCursorPos(closeButtonPos);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));

	if (ImGui::Button(x("X"), ImVec2(12, 15))) {
		bDraw = false;
		exit(0);
	}

	ImGui::SetCursorPos(ImVec2(5, 30));
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void Drawing::Draw()
{
	if (isActive())
	{
		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::PushFont(UI::secondfont);

		ImGui::Begin(x("Bat"), &bDraw, WindowFlags);
		{
			if (!statusDisplayed) {
				Settings::status = x("hf@Klonopin");

				statusDisplayed = true;
			}
			WindowHandle();
			ImGui::BeginGroup();
			static char filePath[256] = "";
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

			ImGui::Text(x("File Path:")); ImGui::SameLine();
			ImGui::InputText(x("##File Path"), filePath, sizeof(filePath));
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Spacing();
				ImGui::Text(x("If the file resides in the same directory as the EXE\nyou only need to type its name without specifying the full directory path.\n# Example: File.bat\n# Example: C:\\Users\\Username\\Desktop\\File.bat"));
				ImGui::Spacing();
				ImGui::EndTooltip();
			}

			static int protections = 1;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

			ImGui::Text(x("Iterations:")); ImGui::SameLine();
			ImGui::SetNextItemWidth(215.f);
			ImGui::InputInt(x("##Iterations"), &protections);
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Spacing();
				ImGui::Text(x("Please specify the number of obfuscation iterations.\n-> For example, choose a value between 1 and 1000.\n+ Increasing the iteration count will result in a larger size."));
				ImGui::Spacing();
				ImGui::EndTooltip();
			}
			if (protections > 2500) {
				protections = 2500;
			}

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 227);

			if (ImGui::Button(x("Obfuscate"))) {
				if (strlen(filePath) > 0) {
					Settings::isObfuscating = true;

					auto obfuscateFunction = [=]() {
						BatObfuscate::createObfuscatedBatchFile(filePath, BatObfuscate::generateRandomString(5) + BatObfuscate::generateRandomFileType(), protections);
						Settings::isObfuscating = false;
						};

					std::thread(obfuscateFunction).detach();

					auto statusFunction = [=]() {
						int dots = 0;
						while (Settings::isObfuscating) {
							std::string statusMessage = std::string(x("Obfuscating"));
							for (int i = 0; i < dots; ++i)
								statusMessage += ".";
							Settings::status = statusMessage;
							std::this_thread::sleep_for(std::chrono::seconds(1));
							++dots;
							dots %= 4;
						}
						};

					std::thread(statusFunction).detach();
				}
				else {
					Settings::status = std::string(x("File path cannot be empty."));
					std::thread(errorBeep).detach();
				}
			}

			ImGui::EndGroup();
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::SetCursorPos(ImVec2(1, 91));
			ImGui::Text(x("Status:"));
			ImGui::SameLine();
			ImGui::Text(Settings::status.c_str());
			drawList->AddLine(ImVec2(windowPos.x + 1, windowPos.y + 90), ImVec2(windowPos.x + windowSize.x, windowPos.y + 90), ImColor(255, 255, 255, 15)); // upper line
			ImGui::SetCursorPos(ImVec2(310, 91));
			ImGui::Text(x("v1.0"));
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Spacing();
				ImVec2 image_size{ 64, 64 };
				ImGui::Image((void*)UI::ImageResource, image_size);
				ImGui::SameLine();
				ImGui::Text(x("Version: 1.0\nCreated By Klonopin\nFor the Users of HackForums\nWith great power comes great responsibility."));
				ImGui::Spacing();
				ImGui::EndTooltip();
			}
		}
		ImGui::PopFont();

		ImGui::End();
	}

#ifdef _WINDLL
	if (GetAsyncKeyState(VK_INSERT) & 1)
		bDraw = !bDraw;
#endif
}