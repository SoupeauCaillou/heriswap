#include "EndMenuStateManager.h"
#include "Game.h"
#include <sstream>
#include <fstream>
#include <string>

EndMenuStateManager::EndMenuStateManager() {

}

void EndMenuStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	
	ADD_COMPONENT(startbtn, Transformation);
	ADD_COMPONENT(startbtn, Rendering);
	ADD_COMPONENT(startbtn, Button);

	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("5.png");
	RENDERING(startbtn)->hide = true;
	RENDERING(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,3);
	
	eScore = theTextRenderingSystem.CreateLocalEntity(5);
	TRANSFORM(eScore)->position = Vector2(1, 6);
	
	eMsg = theTextRenderingSystem.CreateLocalEntity(28);
	TRANSFORM(eMsg)->position = Vector2(-4, 1);
	TEXT_RENDERING(eMsg)->charSize = 0.3;
	TEXT_RENDERING(eMsg)->hide = true;
	TEXT_RENDERING(eMsg)->alignL = true;
	TEXT_RENDERING(eMsg)->color = Color(0.f,0.f,0.f);
}

int EndMenuStateManager::SaveScore() {
	FILE* file = fopen("/home/gautier/c/tilematch/scores.txt", "r");
	char name[50] = "AZEAZEAZ";
	int score=thePlayerSystem.GetScore();
	int place=0;
	//Si le fichier des scores existe deja
	if (file) {
		char nom[10][30];
		int nombre[10];
		int i,j;
		for (i=0;i<10;i++) {
			nom[i][0] = '\0';
			nombre[i] = 0;
		}
		i=0;
		while(i<10 && fscanf(file, "%d%s\n", &(nombre[i]),nom[i])!=EOF) {
			i++;
		}
		fclose(file);
		file = fopen("scores.txt", "w");
		//Si on a pas fait mieux que le 10eme score
		if (i==10 && score < nombre[9]) {
			std::cout << "vous n'avez pas atteint le top 10\n";
			for (int k=0;k<10;k++)
				fprintf(file, "%d%s\n", nombre[k], nom[k]);
		//Sinon on est dans le top 10
		} else {
			j=i;
			i--;
			//On cherche notre place
			while (i>-1 && nombre[i]<score)
				i--;
			printf("%d %d\n", j, i);
			place=i+2;
			//On remet tout dans le fichier en s'inserant
			for (int k=0;k<=i;k++)
				fprintf(file, "%d%s\n", nombre[k], nom[k]);
			fprintf(file, "%d%s\n", score, name);
			for (int k=i+1;k<j;k++)
				fprintf(file, "%d%s\n", nombre[k], nom[k]);
	
			std::cout << name << ":" << score<< " score en "<<i+2<<"eme position\n";
		}
		fclose(file);
	//Sinon nouveau fichier
	} else {
		file = fopen("scores.txt", "w");
		place = 1;
		std::cout << "creation du fichier des scores\n";
		fprintf(file, "%d%s\n", score, name);
		fclose(file);
	}
	return place;
}

void EndMenuStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
		
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
	
	TEXT_RENDERING(eMsg)->hide = false;

	int place = SaveScore();
	
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << thePlayerSystem.GetScore();
	TEXT_RENDERING(eScore)->text = a.str();	
	TEXT_RENDERING(eScore)->hide = false;	
	}
	
	{
	std::stringstream a;
	a.precision(0);
	if (place>0) {
		if (place>1) a << std::fixed << "Vous etes en "<<place<<"eme position";
		else  a << std::fixed << "Vous etes en "<<place<<"ere position";
	} else {
		a << std::fixed << "Noob. T'es meme pas 10eme";
	}
	TEXT_RENDERING(eMsg)->text = a.str();	
	TEXT_RENDERING(eMsg)->hide = false;	
	}
	
	
}

GameState EndMenuStateManager::Update(float dt) {
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return EndMenu;
}
	
void EndMenuStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eMsg)->hide = true;
	RENDERING(startbtn)->hide = true;	
	BUTTON(startbtn)->clicked = true;
	TEXT_RENDERING(eScore)->hide = true;	

}

