#include <stdio.h>
#include <string.h> // для strtok, strcmp, strcpy
#include<locale.h>
#include<ctype.h>
#include<stdlib.h> // для malloc
const int limit = 15; // предел кол-ва пар синонимов

// Обработка слова. Замена всех букв на маленькие, за исключением первой //буквы
void wordProcessing(char word[]); // входные параметры - слово, которое //следует обработать
//Отчистка памяти от массива структур
void Free(struct coupleSynonym Syn[], unsigned char num);// входные параметры - указатель на массив структур, кол-во пар синонимов

// Хранит пары синонимов
struct coupleSynonym
{
	// Указатели на первый символ синонимов
	char* syn1;
	char* syn2;
};

int main()
{
	setlocale(LC_ALL, "Rus");
	unsigned char numCouple = 0; // будет хранить количество пар //синонимов (по условию не больше 15)
	char textString[256]; // строка для чтения файла
	char tempSym; // переменная, будет хранить считанный из файла с //текстом символ
	char sw = 0; // будет показывать, какое слово в строке
	int k = 0;// счетчик символа строки для вывода
	int NumString = 0; // счетчик считанных строк
	int flag = 1;// флаг, показывающий что была считана запятая
	int Check_Out = 0; // будет хранить код ошибки при некорректом //выводе слова(буквы) в файл
	struct coupleSynonym allSyn[limit]; // массив структуры, будет хранить //считанные синонимы по парам


	// Инициализация потока (на считывание синонимов)
	FILE* File_Syn = fopen("Synonyms.txt", "r");

	// Проверить открытия файла
	if (File_Syn == NULL)
	{
		perror("Ошибка открытия файла с синонимами.");
		return-1;
	}

	while (NumString < limit)// подсчет строк в файле. (Не может считать больше 15 строк)
	{
		if (fgets(textString, 256, File_Syn) == NULL)
			break;
		NumString++;// счет считанных строк
	}

	if (ferror(File_Syn) != 0)
	{
		printf("\n Ошибка считывания файла.");
		fclose(File_Syn); // закрыть поток
		return -2;
	}

	if (fseek(File_Syn, 0, SEEK_SET) != NULL)
	{
		printf("\n Не удалось переместить указатель позиции в потоке на начало файла.");
		fclose(File_Syn); // закрыть поток
		return -3;
	}

	while (numCouple < NumString)
	{
		if (fgets(textString, 256, File_Syn) == NULL)
			break;
		k = 0;// обнулить счетчик символов строки
		while (textString[k] != '\0')
		{
			int j = 0; // нужна для считывания слова
			char textWord[50] = "\0"; // массив символов, будет хранить //считанное из файла с текстом обработанное слово

			// Если считанный символ не буква, то пропустить его
			if (!(textString[k] >= 'A' && textString[k] <= 'Z' || textString[k] >= 'a' && textString[k] <= 'z' ||
				textString[k] >= 'А' && textString[k] <= 'я' || textString[k] == 'ё' ||
				textString[k] == 'Ё' || textString[k] == '-')) {
				if (textString[k] == ',')//если считалась запятая,  поставить флаг
					flag = 1;
				k++; // перейти к следующему символу
				continue; // перейти к проверке считанного символа
			}

			// Пока считываются буквы, запомнить их
			while (textString[k] >= 'A' && textString[k] <= 'Z' || textString[k] >= 'a' && textString[k] <= 'z' ||
				textString[k] >= 'А' && textString[k] <= 'я' || textString[k] == 'ё' ||
				textString[k] == 'Ё' || textString[k] == '-')
			{
				textWord[j] = textString[k]; // записать символ
				k++; // перейти к следующему символу
				j++; // увеличить счётчик
			}

			if (textWord[j - 1] == '-' && (j - 1) == 0) // считан только дефис
			{
				continue;
			}

			wordProcessing(textWord); // обработать полученное слово
			if (flag == 1)//проверка на наличие запятой между словами
			{
				if (sw == 0)
				{
					allSyn[numCouple].syn1 = (char*)malloc(sizeof(allSyn[numCouple].syn2) * j);// выделить память
					if (allSyn[numCouple].syn1 == NULL)// если память не выделилась
					{
						printf("\n Ошибка выделения памяти.");
						Free(allSyn, numCouple);
						fclose(File_Syn); // закрыть поток
						return -5;
					}
					strcpy(allSyn[numCouple].syn1, textWord);//скопировать слово в структуру
					sw = 1;// поставить флаг, что первое слово считано
					flag = 0; //убрать флаг, сигнализирующий о запятой
					continue;
				}
				if (sw == 1)
				{
					allSyn[numCouple].syn2 = (char*)malloc(sizeof(allSyn[numCouple].syn2) * j);// выделить память
					if (allSyn[numCouple].syn2 == NULL)// если память не выделилась
					{
						printf("\n Ошибка выделения памяти.");
						Free(allSyn, numCouple);
						fclose(File_Syn); // закрыть поток
						return -6;
					}
					strcpy(allSyn[numCouple].syn2, textWord);//скопировать слово в структуру
					numCouple++;
					sw = 0;
					flag = 0;//убрать флаг, сигнализирующий о запятой
				}
			}
			else
			{
				printf("Ошибка считывания файла с синонимами. Неверный формат записи синонимов!");
				printf("\n Отсутствует запятая между синонимами.");
				Free(allSyn, numCouple);
				fclose(File_Syn); // закрыть поток
				return -7;
			}
		}
	}

	if (ferror(File_Syn) != 0)
	{
		printf("\n Ошибка считывания файла.");
		Free(allSyn, numCouple);
		fclose(File_Syn); // закрыть поток
		return -8;
	}


	fclose(File_Syn); // закрыть поток

	// Инициализация потока (на считывание текста)
	FILE* in = fopen("in.txt", "r");
	// Проверка открытия файла
	if (in == NULL)
	{
		perror("Ошибка открытия файла с исходным текстом.");
		Free(allSyn, numCouple);
		return-9;
	}

	// Инициализация потока (на вывод обработанного текста)
	FILE* out = fopen("out.txt", "w");
	if (out == NULL)
	{
		perror("Ошибка открытия файла для вывода обработанного текста.");
		Free(allSyn, numCouple);
		fclose(in); // закрыть поток
		return-10;
	}

	while (fgets(textString, 256, in) != NULL)
	{
		k = 0;// обнулить счетчик символов строки
		while (textString[k] != '\0')
		{
			int j = 0; // нужна для считывания слова
			char textWord[50] = "\0"; // массив символов, будет хранить //считанное из файла с текстом обработанное слово
			char textWordCopy[50]; // массив символов, будет хранить //считанное из файла с текстом оригинальное слово
			sw = 0; // будет показывать, совпало ли прочитанное слово с //одним из синонимов

			// Если считанный символ не буква, то вывести его в файл
			if (!(textString[k] >= 'A' && textString[k] <= 'Z' || textString[k] >= 'a' && textString[k] <= 'z' ||
				textString[k] >= 'А' && textString[k] <= 'я' || textString[k] == 'ё' ||
				textString[k] == 'Ё' || textString[k] == '-')) {
				// вывести символ в файл и получить код ошибки
				Check_Out = fputc(textString[k], out);

				// Проверка вывода слова в файл
				if (Check_Out == EOF)
				{
					perror("Ошибка вывода слова в файл.");
					Free(allSyn, numCouple);
					fclose(in); // закрыть поток
					fclose(out); // закрыть поток
					remove("out.txt"); // удалить файл с некорректной информацией
					return-11;
				}
				k++; // перейти к следующему символу
				continue; // перейти к проверке считанного символа
			}

			// Пока считываются буквы, запомнить их
			while (textString[k] >= 'A' && textString[k] <= 'Z' || textString[k] >= 'a' && textString[k] <= 'z' ||
				textString[k] >= 'А' && textString[k] <= 'я' || textString[k] == 'ё' ||
				textString[k] == 'Ё' || textString[k] == '-')
			{
				textWord[j] = textString[k]; // записать символ
				k++; // перейти к следующему символу
				j++; // увеличить счётчик
			}

			if (textWord[j - 1] == '-' && (j - 1) == 0) // считан дефис
			{
				// вывести символ в файл и получить код ошибки
				Check_Out = fputc('-', out);

				// Проверка вывода слова в файл
				if (Check_Out == EOF)
				{
					perror("Ошибка вывода слова в файл.");
					Free(allSyn, numCouple);
					fclose(in); // закрыть поток
					fclose(out); // закрыть поток
					remove("out.txt"); // удалить файл с некорректной информацией
					return-12;
				}
				continue;
			}

			strcpy(textWordCopy, textWord); // запомнить оригинальное слово
			wordProcessing(textWord); // обработать полученное слово

			// Сравнение считанного слова с синонимами
			for (int t = 0; t < numCouple; t++) {
				// Если для считанного символа существует синоним, //вывести его в файл
				if (strcmp(textWord, allSyn[t].syn1) == 0)
				{
					// вывести обработанный синоним в файл и получить //код ошибки
					Check_Out = fputs(allSyn[t].syn2, out);
					// Проверка вывода слова в файл
					if (Check_Out == EOF)
					{
						perror("Ошибка вывода слова в файл.");
						Free(allSyn, numCouple);
						fclose(in); // закрыть поток
						fclose(out); // закрыть поток
						remove("out.txt"); // удалить файл с некорректной информацией
						return-13;
					}
					sw = 1; // отметить, что слово синоним
					break;
				}

				// Если для считанного символа существует синоним, //вывести его в файл
				if (strcmp(textWord, allSyn[t].syn2) == 0)
				{
					// вывести обработанный синоним в файл и получить //код ошибки
					Check_Out = fputs(allSyn[t].syn1, out);
					// Проверка вывода слова в файл
					if (Check_Out == EOF)
					{
						perror("Ошибка вывода слова в файл.");
						Free(allSyn, numCouple);
						fclose(in); // закрыть поток
						fclose(out); // закрыть поток
						remove("out.txt"); // удалить файл с некорректной информацией
						return-14;
					}
					sw = 1; // отметить, что слово синоним
					break;
				}
			}

			// Если считанное слово не синоним, то вывести его в файл //(оригинальное слово)
			if (sw == 0)
			{
				// вывести слово и получить код ошибки
				Check_Out = fputs(textWordCopy, out);
				// Проверка вывода слова в файл
				if (Check_Out == EOF)
				{
					perror("Ошибка вывода слова в файл.");
					Free(allSyn, numCouple);
					fclose(in); // закрыть поток
					fclose(out); // закрыть поток
					remove("out.txt"); // удалить файл с некорректной информацией
					return -15;
				}
			}
		}
	}

	if (ferror(in) != 0)
	{
		printf("\n Ошибка считывания файла.");
		Free(allSyn, numCouple);
		fclose(in); // закрыть поток
		fclose(out); // закрыть поток
		remove("out.txt"); // удалить файл с некорректной информацией
		return -16;
	}

	Free(allSyn, numCouple);
	fclose(in); // закрыть поток
	fclose(out); // закрыть поток
	printf("\nПрограмма успешно завершена.\n");
	return 0;
}

// Замена всех букв на маленькие, за исключением первой буквы
void wordProcessing(char word[])
{
	unsigned int i = 0; // счётчик
	// Пока не найден конец слова, делать:
	while (word[i] != '\0')
	{
		// Если это первая буква в слове и она маленькая, то сделать её //большой
		if (i == 0)
		{
			word[i] = toupper(word[i]); // заменить маленькую букву на большую
			// Переход к следующей букве
			i++;
			continue;
		}

		// Если текущая буква не первая в слове и она большая, то сделать //её маленькой
		else
		{
			word[i] = tolower(word[i]); // заменить маленькую букву на большую
			i++;// Переход к следующей букве
			continue;
		}
	}
}

void Free(struct coupleSynonym allSyn[], unsigned char num)
{
	for (int i = 0; i < num; i++)
	{
		free(allSyn[i].syn1 = NULL);
		free(allSyn[i].syn2 = NULL);
	}
}