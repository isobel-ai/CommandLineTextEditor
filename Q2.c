// Assume file names have no spaces as spaces are used to seperate command words and arguments.
// Asssume all lines end with \n so all new lines can be detected.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Initialise global constants and variables
const int MAX_COMMAND_LENGTH = 51; /* Assume any command entered by the user is up to 50 characters as the command words are up
									* to 7 characters and file names aren't usually very long */
const int MAX_LINE_LENGTH = 151; // Assume any line entered by the user is up to 150 characters to limit line length.
const char* TEMP_FILE_NAME = ".temp";
char* CHANGE_LOG_FILE_NAME = ".change_log";
char* INFO_FILE_NAME = ".information";
char clipboard[151]; // Set in lcopy() and used in linsert() when paste mode is on.

// Other Subroutines
int numeric(char* num_string) {
	// Return 1 if the parameter is numeric and 0 otherwise.
	int numeric = 1;

	// Loop through the string and check if each character is numeric.
	int i;
	for (i = 0; i < strlen(num_string); i++) {
		if (! isdigit(num_string[i])) {
			numeric = 0;
			break;
		}
	}
	return numeric;
}

int replace(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int replaced = 0;

	// Try to delete dfile and rename rfile as dfile.
	if (! remove(file_name)) {
		if (! rename(TEMP_FILE_NAME, file_name)) {
			replaced = 1;

		// If file_name has been deleted but the temporary file can't be renamed then output an error message.
		} else {
			printf("Error. %s file lost.", file_name);
		}
	}
	return replaced;
}

int get_line_no(char* file_name) {
	// Return -1 if the operation is unsuccessful and the number of lines in the file otherwise.
	int line_no = -1;

	// Try to open the file for reading. If it works then count the number of lines in the file. After this, close the file.
	FILE* file;
	file = fopen(file_name, "r");
	if(file) {
		char next_line[MAX_LINE_LENGTH];
		line_no = 0;
		while (fgets(next_line, sizeof(next_line), file)) {
			line_no++;
		}
		fclose(file);
	}
	return line_no;
}

void update_log(char* cword, char* args[2]) {
	// Try to open the change log file. If the file isn't opened output an error message.
	FILE* file;
	file = fopen(CHANGE_LOG_FILE_NAME, "a");
	if(! file) {
		printf("The change log couldn't be updated. \n");

	// Otherwise append an appropriate update to the change log then close the file.
	} else {

		if (! strcmp(cword, "create")) {
			fprintf(file, "%s file (%d lines) created. \n", args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "copy")) {
			fprintf(file, "%s file (%d lines) copied to %s file (%d lines). \n", args[0], get_line_no(args[0]), args[1], get_line_no(args[1]));

		} else if (! strcmp(cword, "delete")) {
			fprintf(file, "%s file deleted. \n", args[0]);

		} else if (! strcmp(cword, "show")) {
			fprintf(file, "%s file (%d lines) shown. \n", args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "lappend")) {
			fprintf(file, "line appended to %s file (%d lines). \n", args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "ldelete")) {
			fprintf(file, "line %s deleted from %s file (%d lines). \n", args[1], args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "linsert")) {
			fprintf(file, "line inserted at line number %s in %s file (%d lines). \n", args[1], args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "lshow")) {
			fprintf(file, "line %s from %s file (%d lines) shown. \n", args[1], args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "nshow")) {
			fprintf(file, "%s file's number of lines (%d) shown. \n", args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "find")) {
			fprintf(file, "searched for a string in %s file (%d lines). \n", args[0], get_line_no(args[0]));

		} else if (! strcmp(cword, "lcopy")) {
			fprintf(file, "copied line %s from %s file (%d lines). \n", args[1], args[0], get_line_no(args[0]));

		} else { // if (! strcmp(cword, "lpaste"))
			fprintf(file, "pasted into %s file (%d lines) at line %s. \n", args[0], get_line_no(args[0]), args[1]);
		}
		fclose(file);
	}
	return;
}

// Operation Subroutines
int create(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int created = 0;

	// Check if the file exists by opening it in read mode.
	FILE* file;
	file = fopen(file_name, "r");

	// If the file exsists ask the user if they want to overwrite it.
	char overwrite;
	if (file) {
		printf("%s already exists. Do you want to overwrite it (y/n): ", file_name);
		scanf(" %[^\n]", &overwrite);
	}

	// If the file doesn't exist or the user wants to overwrite it, try to create the file by opening it in write mode.
	if (! file || overwrite == 'y') {
		file = fopen(file_name, "w");

		// If the file isn't created output an error message, otherwise close the file.
		if(! file) {
			printf("%s couldn't be created. \n", file_name);
		} else {
			fclose(file);
			created = 1;
		}

	// If the user doesn't want to overwrite the file, close it. Assume any character that's not 'y' means 'n'.
	} else {
		fclose(file);
	}
	return created;
}

int copy(char* file_names[2]) {
	// Return 1 if the operation is successful and 0 otherwise.
	int copied = 0;

	// Try to open the first file for reading. Output an error message if this fails.
	FILE* rfile;
	rfile = fopen(file_names[0], "r");
	if(! rfile) {
		printf("%s couldn't be opened. It might not exsist. \n", file_names[0]);

	// Otherwise try to open the second file for writing and output an error message and close rfile if it fails.
	} else {
		FILE* wfile;
		wfile = fopen(file_names[1], "w");
		if(! wfile) {
			printf("%s couldn't be created. \n", file_names[0]);
			fclose(rfile);

		// If it doesn't fail then copy each line from rfile to wfile then close both files.
		} else {
			char next_line[MAX_LINE_LENGTH];
			while (fgets(next_line, sizeof(next_line), rfile)) {
				fprintf(wfile, "%s", next_line);
			}
			fclose(rfile);
			fclose(wfile);
			copied = 1;
		}
	}
	return copied;
}

int delete(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int deleted = 0;

	// Check if the file exists by opening it in read mode.
	FILE* file;
	file = fopen(file_name, "r");

	// If the file exists, close it then try to delete it and output an error message if it fails.
	if (file) {
		fclose(file);
		if (remove(file_name)) {
			printf("%s file couldn't be deleted. \n", file_name);
		} else {
			deleted = 1;
		}

	// Otherwise, inform the user that the file doesn't exist.
	} else {
		printf("%s doesn't exist. \n", file_name);
	}
	return deleted;
}

int show(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int shown = 0;

	// Try to open the file for reading. Output an error message if this fails.
	FILE* file;
	file = fopen(file_name, "r");
	if(! file) {
		if (file_name == CHANGE_LOG_FILE_NAME) {
			printf("Change log couldn't be opened. \n");
		} else {
			printf("%s couldn't be opened. It might not exist. \n", file_name);
		}

	// Otherwise output each line in the file then close it.
	} else {
		char next_line[MAX_LINE_LENGTH];
		while (fgets(next_line, sizeof(next_line), file)) {
			printf("%s", next_line);
		}
		fclose(file);
		shown = 1;
	}
	return shown;
}

int lappend(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int lappended = 0;

	// Try to open the file for appending and output an error message if it fails.
	FILE* file;
	file = fopen(file_name, "a");
	if(! file) {
		printf("%s couldn't be appended to. It might not exist. \n", file_name);

	// Otherwise prompt the user to enter their new line.
	} else {
		char newline[MAX_LINE_LENGTH];
		printf("Enter new line: ");
		scanf(" %[^\n]", newline);

		// Append the new line then close the file.
		fprintf(file, "%s\n", newline);
		fclose(file);
		lappended = 1;
	}
	return lappended;
}

int ldelete(char* file_line[2]) { // file_line[0] is file name and file_line[1] is line number.
	// Return 1 if the operation is successful and 0 otherwise.
	int ldeleted = 0;

	// Try to open the file for reading. Output an error message if this fails.
	FILE* rfile;
	rfile = fopen(file_line[0], "r");
	if(! rfile) {
		printf("%s couldn't be opened. It might not exist. \n", file_line[0]);

	// Otherwise try to open a temporary file for writing and output an error message and close rfile if it fails.
	} else {
		FILE* wfile;
		wfile = fopen(TEMP_FILE_NAME, "w");
		if(! wfile) {
			printf("A line couldn't be deleted from %s. \n", file_line[0]);
			fclose(rfile);

		// If it doesn't fail then copy each line except the one to be deleted from rfile to wfile then close both files.
		} else {
			char next_line[MAX_LINE_LENGTH];
			int line_number = atoi(file_line[1]);
			int line_counter = 1;
			while (fgets(next_line, sizeof(next_line), rfile)) {
				if(line_number != line_counter) {
					fprintf(wfile, "%s", next_line);
				}
				line_counter++;
			}
			fclose(rfile);
			fclose(wfile);

			// Check that the line deleted was actually a line in the file. If it was, try to replace the original file with the temporary one.
			if (line_number > 0 && line_number < line_counter) {
				if (replace(file_line[0])) {
					ldeleted = 1;

				// If the original file couldn't be replaced, output an error messgae.
				} else {
					printf("A line couldn't be deleted from %s. \n", file_line[0]);
				}

			// Otherwise output an error message.
			} else {
				printf("Invalid command: line %d is outside the range of file %s. \n", line_number, file_line[0]);
			}
		}
	}
	return ldeleted;
}

int linsert(char* file_line[2], int paste_mode) { // file_line[0] is file name and file_line[1] is line number.
	// Return 1 if the operation is successful and 0 otherwise.
	int linserted = 0;

	// Try to open the file for reading. Output error message if this fails.
	FILE* rfile;
	rfile = fopen(file_line[0], "r");
	if(! rfile) {
		printf("%s couldn't be opened. It might not exist. \n", file_line[0]);

	// Otherwise try to open a temporary file for writing and output an error message and close rfile if it fails.
	} else {
		FILE* wfile;
		wfile = fopen(TEMP_FILE_NAME, "w");
		if(! wfile) {
			printf("A line couldn't be inserted into %s. \n", file_line[0]);
			fclose(rfile);

		// If it doesn't fail then prompt the user to enter their new line if one hasn't been specified.
		} else {
			char newline[MAX_LINE_LENGTH];
			if (paste_mode) {
				snprintf(newline, sizeof(newline), "%s", clipboard);
			} else {
				printf("Enter new line: ");
				scanf(" %[^\n]", newline);
			}

			// Try to copy each line from rfile to wfile, inserting the newline at the specified line number.
			char next_line[MAX_LINE_LENGTH];
			int line_number = atoi(file_line[1]);
			int line_counter = 1;
			while (fgets(next_line, sizeof(next_line), rfile)) {
				if(line_number == line_counter) {
					fprintf(wfile, "%s \n", newline);
				}
				fprintf(wfile, "%s", next_line);
				line_counter++; // Final value is one more than the number of lines in the file.
			}

			// Check if the user was trying to append the line to the file.
			if (line_number == line_counter) {
				fprintf(wfile, "%s \n", newline);
			}

			// Check that the newline was inserted. If it was, try to replace the original file with the temporary one.
			if (line_number > 0 && line_number <= line_counter) {
				if (replace(file_line[0])) {
					linserted = 1;

				// If the original file couldn't be replaced, output an error messgae.
				} else {
					printf("A line couldn't be inserted into %s. \n", file_line[0]);
				}

			// Otherwise output an error message.
			} else {
				printf("Invalid command: line %d is outside the range of file %s. \n", line_number, file_line[0]);
			}

			// Close both files.
			fclose(rfile);
			fclose(wfile);
		}
	}
	return linserted;
}

int lshow(char* file_line[2]) { // file_line[0] is file name and file_line[1] is line number.
	// Return 1 if the operation is successful and 0 otherwise.
	int lshown = 0;

	// Try to open the file for reading. Output error message if this fails.
	FILE* file;
	file = fopen(file_line[0], "r");
	if(! file) {
		printf("%s couldn't be opened. It might not exist. \n", file_line[0]);

	// Otherwise try to output the specified line then close the file.
	} else {
		char next_line[MAX_LINE_LENGTH];
		int line_number = atoi(file_line[1]);
		int line_counter = 1;
		while (fgets(next_line, sizeof(next_line), file)) {
			if(line_number == line_counter) {
				printf("%s", next_line);
			}
			line_counter++;
		}
		fclose(file);

		// Check that a line was shown. If it wasn't, output an error message.
			if (line_number > 0 && line_number < line_counter) {
				lshown = 1;
			} else {
				printf("Invalid command: line %d is outside the range of file %s. \n", line_number, file_line[0]);
			}
	}
	return lshown;
}

int nshow(char* file_name) {
	// Return 1 if the operation is successful and 0 otherwise.
	int nshown = 0;

	// Open the file in read mode to check if it exists. Output an error message if it doesn't exist.
	FILE* file;
	file = fopen(file_name, "r");
	if(! file) {
		printf("%s couldn't be opened. It might not exist. \n", file_name);

	// Otherwise, output how many lines the file has.
	} else {
		int line_no = get_line_no(file_name);
		if (line_no == -1) {
			printf("some number of lines. \n"); // Output if there's an error in get_line_no().
		} else {
			printf("%d lines. \n", line_no);
			nshown = 1;
		}
	}
	return nshown;
}

int find(char* file_string[2]) { // file_string[0] is file name and file_string[1] is search string.
	// Return 1 if the operation is successful and 0 otherwise.
	int found = 1;

	// Try to open the file for reading. Output an error message if this fails.
	FILE* file;
	file = fopen(file_string[0], "r");
	if(! file) {
		if (file_string[0] == CHANGE_LOG_FILE_NAME) {
			printf("change log couldn't be opened. \n");
		} else if (file_string[0] == INFO_FILE_NAME) {
			printf("information about %s couldn't be retrieved. \n", file_string[1]);
		} else {
			printf("%s couldn't be opened. \n", file_string[0]);
		}
		found = 0;

	// Otherwise prompt the user to enter a search string if one hasn't already been provided.
	} else {
		char search_string[MAX_LINE_LENGTH];
		if (! file_string[1]) {
			printf("Enter search string: ");
			scanf(" %[^\n]", search_string);
		} else if (file_string[0] == CHANGE_LOG_FILE_NAME) {
			snprintf(search_string, sizeof(search_string), "%s file", file_string[1]);
		} else { // if (file_string[0] == INFO_FILE_NAME)
			snprintf(search_string, sizeof(search_string), " %s", file_string[1]);
		}

		// Read lines from the file and output them with their line number if they contain the search string.
		char next_line[MAX_LINE_LENGTH];
		int line_counter = 1;
		int found_counter = 0;
		while (fgets(next_line, sizeof(next_line), file)) {
			if (strstr(next_line, search_string)){
				printf("(%d) %s", line_counter, next_line);
				found_counter++;
			}
			line_counter++;
		}

		// Output message if the search string isn't found in the file.
		if (! found_counter) {
			if (file_string[0] == CHANGE_LOG_FILE_NAME) {
				printf("%s not found in change log. \n", search_string);
			} else if (file_string[0] == INFO_FILE_NAME) {
				printf("Invalid command: invalid command word. \n");
			} else {
				printf("Search string not found in file. \n");
			}

		// Otherwise output the found counter if the file isn't the change log or the information file.
		} else if (file_string[0] != CHANGE_LOG_FILE_NAME && file_string[0] != INFO_FILE_NAME) {
			printf("%s found on %d lines. \n", search_string, found_counter);
		}
	}
	return found;
}

int lcopy(char* file_line[2]) { // file_line[0] is file name and file_line[1] is line number.
	// Return 1 if the operation is successful and 0 otherwise.
	int lcopied = 0;

	// Try to open the file for reading. Output an error message if this fails.
	FILE* file;
	file = fopen(file_line[0], "r");
	if(! file) {
		printf("%s couldn't be opened. It might not exist. \n", file_line[0]);

	// Try to store the specified line in a temporary clipboard then close the file.
	} else {
		char next_line[MAX_LINE_LENGTH];
		int line_number = atoi(file_line[1]);
		int line_counter = 1;
		char temp_clipboard[MAX_LINE_LENGTH];
		while (fgets(next_line, sizeof(next_line), file)) {
			if(line_number == line_counter) {
				snprintf(temp_clipboard, sizeof(temp_clipboard), "%s", next_line);
				temp_clipboard[strcspn(temp_clipboard, "\n")] = 0; // Remove newline character from end of line.
				lcopied = 1;
				break;
			}
			line_counter++;
		}
		fclose(file);

		// If a line wasn't stored, output an error message.
		if (! lcopied) {
			printf("Invalid command: line %d is outside the range of file %s. \n", line_number, file_line[0]);

		// Otherwise, clear the clipboard then store the contents of the temporary clipboard in it.
		} else {
			memset(clipboard, 0, sizeof(clipboard));
			snprintf(clipboard, sizeof(clipboard), "%s", temp_clipboard);
		}
	}
	return lcopied;
}

// Main Program
int main() {
	// Introduce the user to the program.
	printf("Text Editor Operations: \n");
	printf("-File Operations: create | copy | delete | show \n");
	printf("-Line Operations: lappend | ldelete | linsert | lshow \n");
	printf("-General Operations: cshow | nshow | help | quit \n");
	printf("-Additional Operations: find | lcopy | lpaste \n");
	printf("For information on how to use all the commands, type 'help'. \n");
	printf("For information on how to use a specific command, type 'help [command]'. \n");
	printf("Note: this text editor only accepts file names without spaces. \n");

	// Initialise variables used in the menu.
	char command[MAX_COMMAND_LENGTH];
	char* cword; // The command word is the first word in the command.
	char* args[3]; // Each command needs up to 2 arguments. Third slot for input validation.
	int i; // For use in loops.

	// Initialise first and second args slots so call to update_log doesn't fail if the command takes no or one argument.
	for (i = 0; i < 2; i++) {
		args[i] = "";
	}

	// Loop menu until the user wants to quit.
	do {

		// Prompt the user to enter a command
		printf("Enter command: ");
		scanf(" %[^\n]", command);

		// Validate command, allowing for re-entry if invalid, then call the appropriate subroutine.
		cword = strtok(command, " ");

		if (! strcmp(cword, "create")) { // Valid command: create [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (create(args[0])) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "copy")) { // Valid command: copy [file_name] [file_name]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0] || ! args[1]) {
				printf("Invalid command: less than 2 file names entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 2 file names entered. \n");
			} else {
				if (copy(args)) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "delete")) { // Valid command: delete [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (delete(args[0])) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "show")) { // Valid command: show [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (show(args[0])) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "lappend")) { // Valid command: lappend [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (lappend(args[0])) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "ldelete")) { // Valid command: ldelete [file_name] [line_number]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 1 file name and 1 line number entered. \n");
			} else if ((! args[1]) || (args[1] && ! numeric(args[1]))) {
				printf("Invalid command: line number not entered. \n");
			} else {
				if (ldelete(args)) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "linsert")) { // Valid command: linsert [file_name] [line_number]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 1 file name and 1 line number entered. \n");
			} else if ((! args[1]) || (args[1] && ! numeric(args[1]))) {
				printf("Invalid command: line number not entered. \n");
			} else {
				if (linsert(args, 0)) { // Paste mode off.
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "lshow")) { // Valid command: lshow [file_name] [line_number]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 1 file name and 1 line number entered. \n");
			} else if ((! args[1]) || (args[1] && ! numeric(args[1]))) {
				printf("Invalid command: line number not entered. \n");
			} else {
				if (lshow(args)) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "cshow")) { // Valid command: cshow OR cshow [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else if (args[0]) {
				char* cargs[2] = {CHANGE_LOG_FILE_NAME, args[0]}; // So the arguments match up with the parameters of find.
				find(cargs);
			} else {
				show(CHANGE_LOG_FILE_NAME);
			}

		} else if (! strcmp(cword, "nshow")) { // Valid command: nshow [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (nshow(args[0])) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "help")) { // Valid command: help OR help [command_word]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else if (args[0]) {
				char* hargs[2] = {INFO_FILE_NAME, args[0]}; // So the arguments match up with the parameters of find.
				find(hargs);
			} else {
				show(INFO_FILE_NAME);
			}

		} else if (! strcmp(cword, "find")) { // Valid command: find [file_name]
			for (i = 0; i < 2; i++) {
				args[i] = strtok(NULL, " ");
			};
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[1]) {
				printf("Invalid command: more than 1 file name entered. \n");
			} else {
				if (find(args)) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "lcopy")) { // Valid command: lcopy [file_name] [line_number]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 1 file name and 1 line number entered. \n");
			} else if ((! args[1]) || (args[1] && ! numeric(args[1]))) {
				printf("Invalid command: line number not entered. \n");
			} else {
				if (lcopy(args)) {
					update_log(cword, args);
				}
			}

		} else if (! strcmp(cword, "lpaste")) { // Valid command: lpaste [file_name] [line_number]
			for (i = 0; i < 3; i++) {
				args[i] = strtok(NULL, " ");
			}
			if (! args[0]) {
				printf("Invalid command: file name not entered. \n");
			} else if (args[2]) {
				printf("Invalid command: more than 1 file name and 1 line number entered. \n");
			} else if ((! args[1]) || (args[1] && ! numeric(args[1]))) {
				printf("Invalid command: line number not entered. \n");
			} else {
				if (linsert(args, 1)) { // Paste mode on.
					update_log(cword, args);
				}
			}

		} else if (strcmp(cword, "quit")) { // Valid command: quit. However, any arguments given are ignored.
			printf("Invalid command: invalid command word entered. \n");
		}
	} while (strcmp(command,"quit"));

	// Output exit message once the user has quit the program.
	printf("Exiting the text editor. \n");

	// Delete the temporary file if it exsists in order to save space on the user's device.
	remove(TEMP_FILE_NAME);
}
