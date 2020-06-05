
void printChar(char c);
void printString(char* chars);
void readString(char* line[80]);	
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);	
int strCompare(char* str1, char* str2);
void readFile(char* buffer, char* name, int* sectorsRead);
void executeProgram(char* name);
void terminate();
void writeSector(char* buffer, int sector);
void deleteFile(char* fileName);
void writeFile(char* buffer, char* fileName, int numberOfSectors);



main ()
{
	//putInMemory(0xB800, 0x140, 'H');
	//putInMemory(0xB800, 0x141, 0x7);

	//step 1
	/*printChar('A');*/

	//step 2 backspace goes too far.
	//char line[80];
	//printString("Enter a line:");
	//readString(line);
	//printString(line);

	//Step 3
	//char buffer[512];
	//readSector(buffer, 30);
	//printString(buffer);

	//step 4 good
	//makeInterrupt21();
	//interrupt(0x21,0,0,0,0);

	//step 5 good
	//char line[80];
	//makeInterrupt21();
	//interrupt(0x21, 1, line, 0, 0);
	//interrupt(0x21, 0, line, 0, 0);

	//ProjectC
	//Step 1
	//char buffer[13312];	//this is the maximum size of a file
	//int sectorsRead;
	//makeInterrupt21();
	//interrupt(0x21, 3, "messag", buffer, &sectorsRead);	//read the file into buffer
	//if(sectorsRead > 0)
	//{
	//	interrupt(0x21, 0, buffer, 0, 0);//print out the file
	//}
	//else
	//{
	//	interrupt(0x21, 0, "messag not found\r\n", 0, 0);	//no sectors read? then print an error
	//}


	//Step 2 load a program and execute it.
	//makeInterrupt21();
	//interrupt(0x21, 4, "tstpr1", 0, 0);

	//step 3 terminate program system call 
	//makeInterrupt21();
	//interrupt(0x21, 4, "tstpr2", 0, 0);

	//ProjectD Step 4 - Writing a File.
	//in your kernel, beofre runnng the shell, write a test line.
	makeInterrupt21();
	interrupt(0x21, 8, "this is a test message", "testmg", 3);

	//step 4 the shell - making your own user program.
	makeInterrupt21();
	interrupt(0x21, 4, "shell", 0);

	while(1)	//this is the hangup.
	{
	}
}

void printChar(char c)
{
	interrupt(0x10, 0xe*256 + c, 0, 0,0);	//print the single char input.
}

void printString(char* chars)
{
	while(*chars != 0x0)
	{
		interrupt(0x10, 0xe*256 + *chars, 0, 0, 0);	//print the characte in the address.
		chars++;	//move the address foreward 1
	}
}

void readString(char* chars)
{
	int startingAddress = chars;
	int isEnterPressed = 0;
	char in;
	while(isEnterPressed == 0)
	{
		in = interrupt(0x16, 0, 0, 0, 0);	//save the keyboard key as a character
		//*chars = interrupt(0x16, 0, 0, 0, 0);	//save the keyboard key as a character
		/*handle the backspace*/
		if(in == '\b')//if in char is '\b'
		{
			if(chars != startingAddress)
			{
			interrupt(0x10, 0xe*256 + in, 0, 0,0);	//print the character previously saved.
			interrupt(0x10, 0xe*256 + ' ', 0, 0,0);	//print the space where the \b would be.
			interrupt(0x10, 0xe*256 + '\b', 0, 0,0);//backup the cursor on the screen.
			interrupt(0x10, 0xe*256 + ' ', 0, 0,0);	//print ' ' at - 1 at the cursor
			interrupt(0x10, 0xe*256 + '\b', 0, 0,0);//backup the cursor to the "empty space".
			chars--;	//move the address back 1
			chars--;	//move the address back a second index to account for the increment at the end.
			}

			else
			{
				chars--;
				//then do nothing
			}
		}
		else
		{
			*chars = in; //interrupt(0x16, 0, 0, 0, 0);	//save the keyboard key as a character
			interrupt(0x10, 0xe*256 + *chars, 0, 0,0);	//print the character previously saved.
		}

		/*handle the ENTER key*/
		if(*chars == '\r')
		{
			chars++;	//move foreward in the array.
			*chars = 0xa;
			chars++;	//move foreward in the array
			*chars = '\n';	//make a new line
			interrupt(0x10, 0xe*256 + *chars, 0, 0,0);	//print a new linefrom the last char
			chars++;	//move foreward in the array
			*chars = '\0';	//add the 0x0 to the end of the array to signal the end.
			isEnterPressed = 1;
		}

	chars++;//move the address foreward 1.
	}
}


void readSector(char* buffer, int sector)
{
	//used to cycle through the char array buffer
	//and read the char at each address into the buffer
	//use interrupt 0x13 interrupt(0x13,AX, BX, CX, DX)
	//AX = 2*256+1, BX = char* array CX = 0+sector+1 DX = 0+ 0x80
	interrupt(0x13, 2*256+1, buffer, 0+sector+1, 0+0x80);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	if(ax == 0)
	{
		printString(bx);
	}
	else if(ax == 1)
	{
		readString(bx);
	}
	else if(ax == 2)
	{
		readSector(bx, cx);
	}
	else if(ax == 3){
		readFile(bx, cx, dx);
	}
	else if(ax == 4)
	{
		executeProgram(bx);
	}
	else if(ax == 5)
	{
		terminate();
	}
	else if(ax == 6)
	{
		writeSector(bx, cx);
	}
	else if(ax == 7)
	{
		deleteFile(bx);
	}
	else if(ax == 8)
	{
		writeFile(bx, cx, dx);
	}
}

// Fake boolean. 1 if same. 0 if different.
int strCompare(char* name, char* dir)
{
	int index;
	for(index = 0; index < 6; index++)
	{
		if(name[index] != dir[index])
		{
			return 0;
		}
		if(name[0 + index] == '\0')
		{
			return 1;
		}
	}
	return 1;
}

void readFile(char* name, char* buffer, int* sectorsRead)
{
	int sector;
	int fileEntry;	//outter loop itterator.
	int i;	//inner loop itterator.	
	int isSameString;	//boolean var.
	int j;
	char dir[512];
	char tempDir[6];
	*sectorsRead = 0;
	readSector(dir, 2);	//load the the directory secot into a 512 bye char* array 
	for(fileEntry = 0; fileEntry < 512; fileEntry = fileEntry +32)	//cycle through the lines of sector 2 "the directory"
	{
		for(j = 0; j < 6; j++)
		{
			tempDir[j] = dir[fileEntry + j];
		}
		isSameString = strCompare(name, tempDir);	//store 0 or 1 into var.depending on if the string is the same.
		if(isSameString == 0)
		{
			*sectorsRead = 0;
		}

		if(isSameString == 1)
		{
			for(i = fileEntry + 6; i < fileEntry + 32; i++)
			{
				readSector(buffer, dir[i]);
				buffer += 512;
				*sectorsRead = *sectorsRead + 1;
			}
			return;
		}
	}
	return;
}


void executeProgram(char* name)
{
	int i; //itterator.
	int sectorsRead;
	char buffer[13312];	//buffer for readFile().

	readFile(name, buffer, &sectorsRead);	//read a file into a buffer.
	if(sectorsRead == 0)
	{
		return;
	}
	for(i = 0; i < 13312; i++)	//in a loop, transfer the file from the buffer into memory at segment 0x2000.
	{
			putInMemory(0x2000, i, buffer[i]);
	}
	launchProgram(0x2000);
}


void terminate()
{

	char shellName[6];
	shellName[0] = 's';
	shellName[1] = 'h';
	shellName[2] = 'e';
	shellName[3] = 'l';
	shellName[4] = 'l';
	shellName[5] = '\0';

	//terminate should no longer hang up but should load and execute shell.
	//while(1)
	//{
	//}
	//projectC step 4  the shell - making your own user program.
	//makeInterrupt21();
	//interrupt(0x21, 4, shellName, 0);
	executeProgram(shellName);
}


//ProjectD
//Step 1: Write Sector
void writeSector(char* buffer, int sector)
{
	//writing sectors is provided by the same BIOS call as readSector.
	//AX = 3 * 256+1, BX = char* array CX = 0+sector+1 DX = 0+ 0x80
	interrupt(0x13, 3 * 256+1, buffer, 0+sector+1, 0+0x80);
}

//ProjectD
//Step 3: Delete File
void deleteFile(char* fileName)
{
	char dir[512];
	char map[512];
	char testArrayDel[7];
	int isSameString;
	int i; 
	int j;
	int k;
	int dirInt;
	//load the Directory and Map to 512 byte character arrays dir and map.
	readSector(dir, 2);
	readSector(map, 1);
		//printString(testArrayDel);
		//printString("\r\n");
	//search throught the directory and try to find he file name
		for(i = 0; i < 512; i = i + 32)	//going through the dir line by line.
		{
			//printString(testArrayDel);
			//printString("\r\n");
			for(j = 0; j < 6; j++)	//going char by char at each line.
			{
				testArrayDel[j] = dir[i + j];	//save the char at index jin dir to fileName to print.
				//printString(testArrayDel);
			}
			//printString(testArrayDel);
			//bool us if the filename matches the one in the Directory.
			isSameString = strCompare(fileName, testArrayDel); 
			if(isSameString == 1)
			{
				//printChar('+');
				//printString("File found!");
				//set the beginning index of the name of the file to '\0'.
				dir[i] = 0x00; 
				//cylce through the rest of the line where the name matches. it starts at [6] to skip over the file's name.
				for(k = 6; k < 32; k++)
				{
					//if it is not '\0' then change it to that.need to use i + k becuase it's the whole spot in the Directory.
					if(dir[i + k] != 0x00)
					{
						//change the map index corresponding to the sector to 00 before setting dir to '\0'
						dirInt = dir[i + k];
						map[dirInt] = 0x00;
						dir[i + k] = 0x00;
						//printChar('!');
						//printString("File deleted. \r\n");
					}
				}
			}
			else
			{
				//printChar('-');
				//printString("File not found.");
			}
		}
		//write the character arrays holding the Directory and Map back to their appropriate sectors.
		writeSector(dir, 2);
		writeSector(map, 1);
}

//Project D Step 4: Writing a file
void writeFile(char* buffer, char* fileName, int numberOfSectors)
{
	char dir[512];
	char map[512];
	int i; //outter directory loop.
	int j; //inner directory loop.
	int k; //outter map loop.
	//load the Directory and Map to 512 byte character arrays dir and map.
	readSector(dir, 2);//read directory into dir array.
	readSector(map, 1);//read map into map array.

	//search throught the directory and try to find a free entry.
		for(i = 0; i < 512; i = i + 32)	//going through the dir line by line.
		{
			if(dir[i] == '\0')
			{
				for(j = 0; j < 6; j++)	//going char by char at each line only if an empty line was found.
				{
					if(fileName[j] == '\0')
					{
						dir[i + j] = '\0';	//If the name is less than 6 bytes, fill the remaining bytes witrh '\0'.
					}
					else
					{
					dir[i + j] = fileName[j];	//copy the name to that directory entry. 
					if(fileName[j] == '\0')
					{
						dir[i + j] = 0x00;
					}
					}
				}
				//for each sector making up the file:
				//find a free sector by searching through the map for 0. Start at entry 3. DO NOT overwrite the bootloader.
				for(k = 3; k < 512; k++)
				{
					if(map[k] == 0x00)	//if the position in the map == 0x00.
					{
						map[k] = 0xFF;// set that sector to 0xFF. It is now taken.
						//Add that sector number to the files directory entry.
						dir[i + j] = k;	//add that sector number ti the file's directory entry.
						writeSector(buffer, dir[i + j]);//write 512 bytes from the buffer holding the file to the sector.
						break;	//stop the for loop if we find the 0x00 in map.
					}					
				}
				break;	//stop the outter loop if we find the 0x00 in directory.				
			}
		}
		//write the character arrays holding the Directory and Map back to their appropriate sectors.
		writeSector(dir, 2);
		writeSector(map, 1);	
}