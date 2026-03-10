#pragma once

#include "../includes.hpp"
#include "record_layer.hpp"
#include <locale>
#include <string>
#include <ctime>

class MacroCell : public CCNode {
	std::string name;
	std::filesystem::path path;
	std::time_t date;

	CompatPopup<>* menuLayer = nullptr;
	CompatPopup<>* mergeLayer = nullptr;
	CCLayer* loadLayer = nullptr;

	bool isMerge = false;
	
public:

	CCMenu* menu = nullptr;
	CCMenuItemToggler* toggler = nullptr;

	static MacroCell* create(std::filesystem::path path, std::string name, std::time_t date, CompatPopup<>* menuLayer, CompatPopup<>* mergeLayer, CCLayer* loadLayer);

	bool init(std::filesystem::path path, std::string name, std::time_t date, CompatPopup<>* menuLayer, CompatPopup<>* mergeLayer, CCLayer* loadLayer);

	void onLoad(CCObject*);

	void handleLoad();

	void onDelete(CCObject*);

	void deleteMacro(bool reload);

	void onSelect(CCObject*);

	void selectMacro(bool single);
};

class LoadMacroLayer : public CompatPopup<CompatPopup<>*, CompatPopup<>*, bool>, public TextInputDelegate {
public:

	CompatPopup<>* menuLayer = nullptr;
	CompatPopup<>* mergeLayer = nullptr;
	CCMenu* menu = nullptr;

	CCMenuItemToggler* selectAllToggle = nullptr;
	CCMenuItemToggler* sortToggle = nullptr;

	CCMenuItemToggler* p1Toggle = nullptr;
	CCMenuItemToggler* p2Toggle = nullptr;
	CCMenuItemToggler* owToggle = nullptr;

	CCMenuItemSpriteExtra* searchOff = nullptr;
	TextInput* searchInput = nullptr;

	CCLabelBMFont* macroCountLbl = nullptr;

	std::vector<MacroCell*> selectedMacros;
	std::vector<MacroCell*> allMacros;
	std::string search = "";

	bool isAutosaves = false;
	bool isMerge = false;
	bool invertSort = false;

	static LoadMacroLayer* create(CompatPopup<>* layer, CompatPopup<>* layer2, bool autosaves);

	bool setup(CompatPopup<>* layer, CompatPopup<>* layer2, bool autosaves) override;

	static void open(CompatPopup<>* layer, CompatPopup<>* layer2, bool autosaves = false);

	void openFolder(CCObject*) {
		file::openFolder(Mod::get()->getSettingValue<std::filesystem::path>(isAutosaves ? "autosaves_folder" : "macros_folder"));
	}

	void textChanged(CCTextInputNode* p) override;

	void clearSearch(CCObject*);

	void addList(bool refresh = false, float prevScroll = 0.f);

	void reloadList(int amount = 1);

	void deleteSelected(CCObject*);

	void onSelectAll(CCObject*);

	void onImportMacro(CCObject*);

	void updateSort(CCObject*);

	void closeOnNextFrame(float);
};
