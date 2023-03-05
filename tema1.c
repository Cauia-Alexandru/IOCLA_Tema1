#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir
{
	char *name;
	struct Dir *parent;
	struct File *head_children_files;
	struct Dir *head_children_dirs;
	struct Dir *next;
} Dir, *TDir;

typedef struct File
{
	char *name;
	struct Dir *parent;
	struct File *next;
} File, *TFile;

TDir AlocDirector(char *nume)
{
	TDir aux = (TDir)malloc(sizeof(Dir));
	if (aux)
	{
		aux->name = malloc((strlen(nume) + 1) * sizeof(char));
		strcpy(aux->name, nume);
		aux->head_children_dirs = NULL;
		aux->head_children_files = NULL;
		aux->next = NULL;
		aux->parent = NULL;
	}
	return aux;
}

TFile AlocFile(char *nume)
{
	TFile aux = (TFile)malloc(sizeof(File));
	if (aux)
	{
		aux->name = malloc((strlen(nume) + 1) * sizeof(char));
		strcpy(aux->name, nume);
		aux->next = NULL;
		aux->parent == NULL;
	}
	return aux;
}

void freeFile(TFile file)
{
	if(!file) return;
	free(file->name);
	free(file);
	file = NULL;
}

void freeDir(TDir dir)
{
	if(!dir) return;
	free(dir->name);
	free(dir);
	dir = NULL;
}

void touch(Dir *parent, char *nume)
{

	if (parent->head_children_files == NULL)         	//daca directorul nu are fisiere
	{
		TFile fisier_nou = AlocFile(nume);
		parent->head_children_files = fisier_nou; 		//creez fisierul, parent va pointa la el
		fisier_nou->parent = parent;                    //fisierul pointeaza la parent
	}
	else
	{
		TFile fisier2 = parent->head_children_files;
		while (fisier2 != NULL)
		{
			if (strcmp(fisier2->name, nume) == 0)
			{ 											//caut fisierul cu numele primit ca parametru in lista
				printf("File already exists\n");		//daca exista afisez mesajul
				return;
			}
			fisier2 = fisier2->next;
		}

		TFile fisier_nou = AlocFile(nume);
		fisier2 = parent->head_children_files;
		while (fisier2->next != NULL) 					//daca nu exista fisier cu numele cautat, parcurg lista pana la sfarsit si 
														//il inserez
		{
			fisier2 = fisier2->next;
		}
		fisier2->next = fisier_nou;
	}
}

void mkdir(Dir *parent, char *nume)
{

	if (parent->head_children_dirs == NULL)
	{
		TDir director_nou = AlocDirector(nume);
		parent->head_children_dirs = director_nou;
		director_nou->parent = parent;
	}
	else
	{
		TDir dir2 = parent->head_children_dirs;
		while (dir2 != NULL)
		{
			if (strcmp(dir2->name, nume) == 0)
			{
				printf("Directory already exists\n");
				return;
			}
			dir2 = dir2->next;
		}
		TDir director_nou = AlocDirector(nume);
		dir2 = parent->head_children_dirs;
		while (dir2->next != NULL)
		{
			dir2 = dir2->next;
		}
		dir2->next = director_nou;
		director_nou->parent = parent;
	}
}

void ls(Dir *parent)
{
	if (parent->head_children_dirs != NULL)
	{
		TDir dir = parent->head_children_dirs;  
		while (dir != NULL)             			//cat timp nu ajung la null
		{											//printez numele si avansez
			printf("%s\n", dir->name);
			dir = dir->next;
		}
	}
	if (parent->head_children_files != NULL)
	{
		TFile file = parent->head_children_files;
		while (file != NULL)
		{
			printf("%s\n", file->name);
			file = file->next;
		}
	}
}

void rm(Dir *parent, char *nume)
{
	if (parent->head_children_files != NULL)
	{
		TFile ant = NULL, aux = parent->head_children_files, head = aux; //declar 2 pointer ajutatori ca sa execut eliminarea
		while (aux != NULL)
		{
			if (strcmp(aux->name, nume) == 0)                             //daca gasesc numele
			{
				if (ant == NULL)                                         // daca e primul in lista
				{
					head = aux->next;
					freeFile(aux);                                        //eliminare + refacerea legaturilor
					parent->head_children_files = head;
					return;
				}
				else
				{
					ant->next = aux->next;
					freeFile(aux);
					return;
				}
			}
			else
			{
				ant = aux;
				aux = aux->next;                                            //avansez in lista 
			}
		}
	}
	else
	{
		printf("Could not find the file\n");
		return;
	}

	printf("Could not find the file\n");
}

void rmdir(Dir *parent, char *nume)
{
	if (parent->head_children_dirs != NULL)                              //verific daca directorul curent contine alti directori
	{
		TDir dir = parent->head_children_dirs, ant = NULL, head = dir;
		while (dir != NULL)                                              //cat timp exista lista
		{
			if (strcmp(dir->name, nume) == 0)                            //compar numele ca sa gasesc directorul care trebuie sters 
			{
				if (ant == NULL)
				{
					head = dir->next;
					parent->head_children_dirs = head;                   //eliminare cand dir e primul in lista
					break;
				}
				else if (ant != NULL && dir->next == NULL)
				{
					ant->next = NULL;									 //eliminare cand e ultimul
					break;
				}
				else
				{
					ant->next = dir->next;								 //eliminare de la mijloc
					break;
				}
			}
			else
			{
				ant = dir;
				dir = dir->next; 										 //caut mai departe in lista
			}
		}
		if (dir == NULL)												//daca dir a ajuns null, inseamna ca a parcurs toata lista si nu l-a gasit
		{
			printf("Could not find the dir\n");
			return;
		}
																		//dupa ce am obtinut directorul aparte, sterg toata informatia din el
		if (dir->head_children_files != NULL)
		{
			TFile file = dir->head_children_files, head = file;
			while (file != NULL)
			{
				head = file->next;										//sterg file-urile si eliberez memoria
				freeFile(file);
				dir->head_children_files = head;
				file = head;
			}
		}
		if (dir->head_children_dirs != NULL)
		{
			TDir dir2 = dir->head_children_dirs, dir_aux = dir2;
			while (dir2 != NULL)
			{
				dir_aux = dir2->next;
				rmdir(dir, dir2->name); 							//directoarele la randul lor pot avea alte fisiere sau directoare, apelez recursiv
				dir->head_children_dirs = dir_aux;
				dir2 = dir_aux;										//avansez in lista pana ajung la null
			}
		}
		freeDir(dir);
	}
	else
	{
		printf("Could not find the dir\n");
		return;
	}
}

void cd(Dir **target, char *nume)
{
	if (strcmp(nume, "..") != 0)                        //verific daca nu trebuie sa plec in directorul parinte
	{
		if ((*target)->head_children_dirs != NULL)       //daca directorul contine directoare
		{
			TDir dir = (*target)->head_children_dirs;
			while (dir != NULL)                          //cat timp nu se termina lista de directoare
			{
				if (strcmp(dir->name, nume) == 0)        //caut directorul cu numele din parametru
				{
					(*target) = dir;                     // daca il gasesc, mut targetul
					return;
				}
				else if (dir->next != NULL)
				{
					dir = dir->next;                      //caut pana la finalul listei
				}
				else
				{
					printf("No directories found!\n");
					break;
				}
			}
		}
	}

	if ((*target)->parent != NULL)
	{                                                    //plec in directorul parinte
		(*target) = (*target)->parent;
	}
}

char *pwd(Dir *target) {
	char* str;
	char* str2 = malloc(sizeof(char));
		while(target != NULL)
		{
			str = (char*)malloc(strlen(str2) + strlen(target->name) + 2);
			strcat(str, "/");													// in str introduc "/"
			strcat(str, target->name);											// concatenez "/" cu numele
			strcat(str, str2);													// la sirul de caractere anterior, adaug sirul curent
			free(str2);
			target = target->parent;											// avansez
			str2 = str;
			
		}
	return str2;
}

void stop(Dir *target) {
	while (target->parent != NULL)
	{
		target = target->parent;
	}
	

	TDir director = target->head_children_dirs, aux;

	while(director != NULL)
	{
		aux = director->next;
		rmdir(target, director->name);
		director = aux;

	}
	TFile file = target->head_children_files, aux_file;
	while(file != NULL)
	{
		aux_file = file->next;
		rm(target, file->name);
		file = aux_file;
	}

	free(target->name);
	free(target);
	
}

void tree(Dir *target, int level) {
	if(target->head_children_dirs != NULL) //verific daca directorul curent contine alti directori
	{
		TDir dir = target->head_children_dirs;
		while(dir != NULL) 
		{
			int i = level * 4;
			while(i > 0)
			{							//printez spatiile necesare in dependenta de i
				printf(" ");
				i--;
			}
			printf("%s\n", dir->name); //printez numele primului director
			tree(dir, level + 1); //avansez in ierarhie recursiv
			dir = dir->next; //trece la urmatorul director
			
		}
	}

	if(target->head_children_files != NULL)
	{
		TFile file = target->head_children_files;
		while(file != NULL)
		{
			int i = level * 4;
			while(i > 0)
			{
				printf(" ");
				i--;
			}
			printf("%s\n", file->name);
			file = file->next;
		}
	}
}

void mv(Dir *parent, char *oldname, char *newname) {
	int dirFound = 0;
	TDir foundDir= NULL;
	TDir ant_dir_aux = NULL;
	TFile ant_file_aux = NULL;
	TFile foundFile = NULL;
	//cautare newname, oldname pentru director
	if(parent->head_children_dirs != NULL)
	{
		TDir dir = parent->head_children_dirs, ant_dir_aux;
		while(dir != NULL)
		{
			if(strcmp(dir->name, oldname) == 0){  //parcurg directoarele si verific daca exista directorul caruia sa-i schimb numele
				foundDir = dir;
				dirFound = 1;
				break;
			}
			dir = dir->next;
			ant_dir_aux = dir;
		}
		if(dir == NULL)                          //daca am parcurs toata lista si ajung la null, nu l-am gasit
		{
			if (parent->head_children_files == NULL)
			{
				printf("File/Director not found\n");
				return;
			}
		}
		TDir dir_aux = parent->head_children_dirs;     //stez pointerul la inceputul listei de directoare
		while(dir_aux != NULL)
		{
			if(strcmp(dir_aux->name, newname) == 0)    //caut pentru newname daca exista
			{
				printf("File/Director already exists\n");
				return;
			}
			dir_aux = dir_aux->next;
		}		

	}
	else
	{
		if (parent->head_children_files == NULL)       //daca nu are directoare, verific si files
		{
			printf("File/Director not found\n");
			return;
		}
	}
	//cautare oldname, newname pentru file
	if(parent->head_children_files != NULL)
	{
		TFile file = parent->head_children_files, ant_file_aux;
		if (dirFound == 0)                                            //daca dir e 1 nu intru, pentru ca deja am cautat pentru director
		{
			while(file != NULL)
			{
				if(strcmp(file->name, oldname) == 0){
					foundFile = file;
					break;
				}
				file = file->next;
				ant_file_aux = file;
			}
			if(file == NULL)
			{
				printf("File/Director not found\n");
				return;
			}
		}
		
		
		TFile file_aux = parent->head_children_files;
		while(file_aux != NULL)
		{
			if(strcmp(file_aux->name, newname) == 0)
			{
				printf("File/Director already exists\n");
				return;
			}
			file_aux = file_aux->next;
		}
		
	}
	else
	{
		if(parent->head_children_dirs == NULL)
		{
			printf("File/Director not found\n");
			return;
		}
	}

	//readaugare director
	if(foundDir != NULL)                                                   //daca a gasit oldname, foundDir pointeaza la directorul gasit
	{
		foundDir->name = realloc(foundDir->name, strlen(newname) + 1);
		strcpy(foundDir->name, newname);
		if(ant_dir_aux == NULL)                                            //elimin in cazul cand e primul in lista
		{
			parent->head_children_dirs = foundDir->next;
			foundDir->next = NULL;
		}
		else
		{
			ant_dir_aux->next = foundDir->next;
			foundDir->next = NULL;                                           //cand nu e primul, elimin
		}
		TDir director = parent->head_children_dirs;
		if(director == NULL){
			parent->head_children_dirs = foundDir;							 //daca e lista goala il fac capul listei 
			return;
		}
		while(director->next != NULL)
		{
			director = director->next;                                       //daca nu, inserez la sfarsit
		}
		director->next = foundDir;
	}
    //readaugare file
	if(foundFile != NULL)
	{
		foundFile->name = realloc(foundFile->name, strlen(newname) + 1);
		strcpy(foundFile->name, newname);
		if(ant_file_aux == NULL)
		{
			parent->head_children_files = foundFile->next;
			foundFile->next = NULL;
		}
		else
		{
			ant_file_aux->next = foundFile->next;
			foundFile->next = NULL;
		}
		TFile fisier = parent->head_children_files;
		while(fisier->next != NULL)
		{
			fisier = fisier->next;
		}
		fisier->next = foundFile;
	}

}

int main()
{
	char *str;
	char* rezultat;
	str = (char *)malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	size_t bufsize;

	TDir home = AlocDirector("home");

	do
	{
		fgets(str, MAX_INPUT_LINE_SIZE, stdin);
		if (str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0';
		if (str[strlen(str) - 1] == '\r')
			str[strlen(str) - 1] = '\0';

		char *comanda = strtok(str, " ");
		if (strcmp(comanda, "touch") == 0)
		{
			char *nume = strtok(NULL, " ");
			touch(home, nume);
		}

		else if (strcmp(comanda, "mkdir") == 0)
		{
			char *name = strtok(NULL, " ");
			mkdir(home, name);
		}

		else if (strcmp(comanda, "ls") == 0)
		{
			ls(home);
		}
		else if (strcmp(comanda, "rm") == 0)
		{
			char *name = strtok(NULL, " ");
			rm(home, name);
		}
		else if (strcmp(comanda, "rmdir") == 0)
		{
			char *name = strtok(NULL, " ");
			rmdir(home, name);
		}
		else if (strcmp(comanda, "cd") == 0)
		{
			char *name = strtok(NULL, " ");
			cd(&home, name);
		}
		else if (strcmp(comanda, "tree") == 0)
			tree(home, 0);
		else if(strcmp(comanda, "pwd") == 0)
		{
			rezultat = pwd(home);
			printf("%s\n", rezultat);
			free(rezultat);
		}
		else if(strcmp(comanda, "mv") == 0)
		{
			char *oldname = strtok(NULL, " ");
			char *newname = strtok(NULL, " ");
			mv(home, oldname, newname);
		}

	} while (strcmp(str, "stop") != 0);
	if(str)
		free(str);
	
	stop(home);

	return 0;
}
