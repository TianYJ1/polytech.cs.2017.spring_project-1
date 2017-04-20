#include "LibraryMerger.h"
char * pathCur;
char levelsPaths[256][128] = { 0 };
extern float SCREEN_WIDTH_UNIT, SCREEN_HEIGHT_UNIT;
int yOffset = 0;
void showDirectoryListing();
int openFolderDialog()
{

	ALLEGRO_FILECHOOSER *dialog = al_create_native_file_dialog("D:/School/Info/sokoban/src/res/","Choose folder","*.*",ALLEGRO_FILECHOOSER_FOLDER);
	if (al_show_native_file_dialog(display, dialog))
	{
		pathCur = al_get_native_file_dialog_path(dialog, 0);
		showDirectoryListing();
	}
	return 1;
}
int openLevelSelect()
{
	changeScene(LEVEL_SELECT_SCENE);
	clearButtons(LEVEL_SELECT_SCENE);
	clearSpritesScene(LEVEL_SELECT_SCENE);
	showDirectoryListing();
	renderScreen();
	showDirectoryListing();
	
	
}
char names[64][BUTTONS_NAME_SIZE];
int levelSelectPage = 0;
void sliceFile()
{
	int c = 0;
	char * pathSource = "", *pathTarget ="";
	ALLEGRO_FILECHOOSER *dialog = al_create_native_file_dialog("D:/School/Info/sokoban/src/res/", "Choose source", "*.txt", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if (al_show_native_file_dialog(display, dialog))
	{
		pathSource = al_get_native_file_dialog_path(dialog, 0);
		dialog = al_create_native_file_dialog("D:/School/Info/sokoban/src/res/", "Choose folder", "*.*", ALLEGRO_FILECHOOSER_FOLDER);
		if (al_show_native_file_dialog(display, dialog))
		{
			pathTarget = al_get_native_file_dialog_path(dialog, 0);
			char destFileStr[256] = { 0 };
			FILE *sourceFile = fopen(pathSource, "r");
			sprintf(destFileStr, "%s/%i.txt", pathTarget, c++);
			FILE *destFile = fopen(destFileStr, "w+");
			if (sourceFile == NULL)
			{
				Log_e(__func__, "Source file error!\n");
				return 1;
			}
			char str[256];
			bool prevStrIsData = false;
			while (fgets(str, sizeof(str), sourceFile) != NULL)
			{
				
				Log_i(__func__, "destFileStr=%s,str=%s", destFileStr,str);
				int occur = 0;
				if (strstr(str,"#"))
					occur++;
				if (strstr(str,"."))
					occur++;
				if (strstr(str,"@"))
					occur++;
				if (strstr(str,"$"))
					occur++;
				if (strstr(str," "))
					occur++;
				if (occur <= 1)
					prevStrIsData = false;
				else
				{
					if (!prevStrIsData)
					{
						sprintf(destFileStr, "%s/%i.txt", pathTarget, c++);
						fclose(destFile);
						destFile = fopen(destFileStr, "w+");
					}
					if (destFile == NULL)
					{
						Log_e(__func__, "destFile file error!\n");
						return 1;
					}
					fprintf(destFile, "%s", str);
					prevStrIsData = true;
				}
			}
			fclose(destFile);
			fclose(sourceFile);

		}
	}
}
int nextPage(int i)
{
	levelSelectPage++;
	clearButtons(LEVEL_SELECT_SCENE);
	clearSpritesScene(LEVEL_SELECT_SCENE);
	showDirectoryListing();
	renderScreen();
}
int prevPage(int i)
{
	levelSelectPage--;
	
	clearButtons(LEVEL_SELECT_SCENE);
	clearSpritesScene(LEVEL_SELECT_SCENE);
	showDirectoryListing();
	renderScreen();
}
void showDirectoryListing()
{
	clearButtons(LEVEL_SELECT_SCENE);
	clearSpritesScene(LEVEL_SELECT_SCENE);
	addButtonSprite("btntile.png", "Change folder", SCREEN_WIDTH_UNIT * 1600, SCREEN_WIDTH_UNIT * 1, SCREEN_WIDTH_UNIT * 400, SCREEN_WIDTH_UNIT * 100, 255, 255, 255, openFolderDialog, LEVEL_SELECT_SCENE, 1, 4);
	addButtonSprite("btntile.png", "->", SCREEN_WIDTH_UNIT * 1100, SCREEN_HEIGHT_UNIT * 1800, SCREEN_WIDTH_UNIT * 100, SCREEN_WIDTH_UNIT * 100, 255, 255, 255, nextPage, LEVEL_SELECT_SCENE, 1, 4);
	addButtonSprite("btntile.png", "<-", SCREEN_WIDTH_UNIT * 900, SCREEN_HEIGHT_UNIT * 1800, SCREEN_WIDTH_UNIT * 100, SCREEN_WIDTH_UNIT * 100, 255, 255, 255, prevPage, LEVEL_SELECT_SCENE, 1, 4);
	ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(pathCur);
	if (al_open_directory(dir))
	{
		ALLEGRO_FS_ENTRY* file;
		int count = 0;
		int(*callBacks[64])(int id);
		
		while (file = al_read_directory(dir))
		{
			if (file == NULL)
				Log_e(__func__, "ERROR: Level file is null");
			else
			{
				//printf("%s\n", al_get_fs_entry_name(file));
				char *path = al_get_fs_entry_name(file);
				char *filename = strrchr(path, '\\');
				if (filename == NULL)
				{
					filename = strrchr(path, '/');
					if (filename == NULL)
						filename = path;
					else
						filename++;
				}
				else
					filename++;
				if (strstr(filename, "."))
				{
					
					if (count >= levelSelectPage * LEVEL_SELECT_ITEMS_PER_PAGE && count < (levelSelectPage + 1) * LEVEL_SELECT_ITEMS_PER_PAGE)
					{
						sprintf(levelsPaths[count- levelSelectPage * LEVEL_SELECT_ITEMS_PER_PAGE], "%s", path);
						filename[strlen(filename) - 4] = 0;
						sprintf(names[count- levelSelectPage * LEVEL_SELECT_ITEMS_PER_PAGE], "%s", filename);
						callBacks[count- levelSelectPage * LEVEL_SELECT_ITEMS_PER_PAGE] = &openLevel;
						Log_i(__func__, "Level: %s", filename);
					}
					count++;
				}
				if (count > (levelSelectPage + 1) * LEVEL_SELECT_ITEMS_PER_PAGE)
					break;
			}
		}
		al_destroy_fs_entry(file);
		makeGridSprites(count- levelSelectPage * LEVEL_SELECT_ITEMS_PER_PAGE-1, 3, names, "btntile.png", SCREEN_WIDTH_UNIT * 40, SCREEN_HEIGHT_UNIT * 500, SCREEN_WIDTH_UNIT * 400, SCREEN_WIDTH_UNIT * 100, callBacks, LEVEL_SELECT_SCENE, 1);
	}
	al_destroy_fs_entry(dir);
	Log_i(__func__, "Folder: %s", pathCur);
}
void Exit()
{
	EventManagerThreadRunning = 0;
}
int main()
{
	if(!DEBUG_MODE)
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	
	initAddons();
	initVars();
	initButtons();
	Log_i(__func__, "Initied\n================\n");
	int(*callBacks[3])(int id) = { openLevelSelect,Exit };
	char names[][BUTTONS_NAME_SIZE] = { "Level Select\0" ,"Exit\0" };
	sprintf(resourcePath, "%s\\Levels\\", resourcePath);
	pathCur = resourcePath;
	addButton("TL", 0, 0, 50, 50, 255, 255, 255, NULL,0,0);
	addButton("TR", SCREEN_WIDTH-50, 0, 50, 50, 255, 255, 255, NULL, MAINMENU_SCENE,0);
	addButton("BL", 0, SCREEN_HEIGHT-50, 50, 50, 255, 255, 255, NULL, MAINMENU_SCENE,0);
	addButton("BR", SCREEN_WIDTH-50, SCREEN_HEIGHT-50, 50, 50, 255, 255, 255, NULL, MAINMENU_SCENE,0);
	
	makeListSprites(2,names, "btntile.png", SCREEN_WIDTH/2-125, 50, 250, 150, callBacks, MAINMENU_SCENE, 1);
	//sliceFile();
	addSprite("back.jpg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, -1, 0);
	Log_i(__func__, "Added button");
	renderScreen();
	initEventManager();//last to be called untill die
	//al_rest(5.0);
	//system("pause");
	//al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_path(path);
	return 0;
    
}
