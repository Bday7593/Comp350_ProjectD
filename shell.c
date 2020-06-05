





int strCompare(char* command, char* shellCommand);

main ()
{
	char buffer[13312];	//this is the maximum size of a file
	char dir[512]; 	//array to hold a sector for dir.
	int sectorsRead;
	char command[4];
	char fileName[20];
	char fileName2[20];
	char dirFileName[7];
	int isCommandSame;
	int isFileNameSame;
	int whileBool;
	int i;
	int j;
	int k;
	int decider = 0;

	//initial shell should run an infinite loop.
	while(1)
	{
		char line[512];
		for(i = 0; i < 512; i++)
		{
			line[i] = '\0';
		}
		
		//on each iteration it should pront a prompt.
		syscall(0,"A:>");	//printString

		//it should then read in a line and match that line to a command.
		syscall(1, line);	//readString
		i = 0;
		whileBool = 0;
		while(whileBool == 0)
		{
			command[i] = line[i]; //store the command portion of line into a char array.
			if(line[i] == '\r')
			{
				whileBool = 1;
				break;
			}
			if(line[i] == 0x20)
			{
				whileBool = 1;
				break;
			}
			i = i + 1;
		}
		//syscall(0, "command");
		//syscall(0, "\r\n");
		//syscall(0, command);	//print string for the command portion
		//syscall(0, "\r\n");
		
		j = 0;
		whileBool = 0;
		while(whileBool == 0)
		{
			if(command[i] == '\r'){
				break;
			}
			fileName[j] = line[j + 1 + i];	//store the filename portion of line into a char array.
			if(fileName[j] == '\r')
			{
				//syscall(0,".");
				break;
			}
			if(fileName[j] == ' ')
			{
				break;
			}
			j = j + 1;
		}		
		//syscall(0, "fileName");
		//syscall(0, "\r\n");
		//syscall(0,fileName);
		//syscall(0, "\r\n");

		k = 0;
		whileBool = 0;
		while(whileBool == 0)
		{
			if(command[i] == '\r'){
				break;
			}
			if(fileName[j] == '\r')
			{
				break;
			}
			fileName2[k] = line[j + 1 + i + 1 + k];	//store the filename portion of line into a char array.
			if(fileName2[k] == '\r')
			{
				//syscall(0,".");
				break;
			}
			if(fileName2[k] == ' ')
			{
				break;
			}
			k = k + 1;
		}		
		//syscall(0, "fileName2");
		//syscall(0, "\r\n");
		//syscall(0,fileName2);
		//syscall(0, "\r\n");
		//test various inputs for the command.
		isCommandSame = strCompare(command, "type");
		if(isCommandSame == 1)
		{
			decider = 1;
		}
		isCommandSame = strCompare(command, "exec");
		if(isCommandSame == 1)
		{
			decider = 2;
		}
		isCommandSame = strCompare(command, "dir");
		if(isCommandSame == 1)
		{
			decider = 3;
		}
		isCommandSame = strCompare(command, "del");
		if(isCommandSame == 1)
		{
			decider = 4;
		}
		isCommandSame = strCompare(command, "copy");
		if(isCommandSame == 1)
		{
			decider = 5;
		}
		isCommandSame = strCompare(command, "create");
		if(isCommandSame == 1)
		{
			decider = 6;
		}

		//compare the shell command to the string "type"
		if(decider == 1)
		{
			syscall(3, fileName, buffer, &sectorsRead);
			if(sectorsRead > 0)
			{
				syscall(0, buffer);	//fileName was found and put into buffer.
				syscall(0, "\r\n");
			}
			else
			{
				syscall(0, "messag not found\r\n");	//fileName was not found.
			}
		}

		//compare shell command to the string "exec"
		if(decider == 2)
		{
			syscall(4, fileName);	//executes program if it is found
			syscall(0, "<PROGRAM NOT FOUND>\r\n");//it will ony get here if program is not found.
		}

		//compare shell command to the string "dir" 
		if(decider == 3)
		{
			syscall(2, dir, 2);	//readSector. store into dir array for sector 2.
			for(i = 0; i < 512; i = i + 32)	//going through the dir line by line.
			{
				for(j = 0; j < 6; j++)	//going char by char at each line.
				{
					dirFileName[j] = dir[j + i];	//save the char at index jin dir to dirFileName to print.
				}

				dirFileName[6] = '\0';	//set the last index of the array to '\0'
				if(dirFileName[0] != '\0')
				{
				syscall(0, dirFileName);	//print dirFileName only if the [0] != ''
				syscall(0, "\r\n");
				}
			}
		} 

		//compare shell command to the string "del"
		if(decider == 4)
		{
			syscall(7, fileName);	//executes program if it is found
		}

		//compare shell command to the string "copy"
		if(decider == 5)
		{
			syscall(3, fileName, buffer, &sectorsRead);	//readSector
			syscall(8, buffer, fileName2, 3);	//writeFile(the buffer goes into a new file named fileName2).
		}

		//compare shell command to string "create"
		if(decider == 6)
		{
			whileBool = 0;
			while(whileBool == 0)	//repeatedly prompt for a line of text until an empty line is entered.
			{
				//on each iteration it should pront a prompt.
				syscall(0," ");	//printString

				//it should then read in a line and match that line to a command.
				syscall(1, buffer);	//readString
				if(buffer[0] == '\r')	//this means an empty line was entered.
				{
					whileBool = 1;
				}
				else
				{
					//if an empty line was not entered, then write the buffer to a file.
					syscall(8, buffer, fileName, 3);	//writeFile. 
				}
			}

		}

		//if nothing has been decided it must be a bad command.
		if(decider == 0)
		{
		syscall(0, "Bad Command!\r\n");	//printString
		}
	}
}


// Fake boolean. 1 if same. 0 if different.
int strCompare(char* command, char* shellCommand)
{
	int index;
	for(index = 0; index < 4; index++)
	{
		if(shellCommand[0 + index] == '\0')
		{
			return 1;
		}
		if(command[index] != shellCommand[index])
		{
			return 0;
		}
		//if(command[0 + index] == '\0')
		//{
		//	return 1;
		//}
	}
	return 1;
}