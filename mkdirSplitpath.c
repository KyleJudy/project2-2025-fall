#include "types.h"
#include <stdbool.h>

extern struct NODE* root;
extern struct NODE* cwd;

struct NODE* endOfDir(struct NODE* startOfDir) {
   while ( startOfDir->siblingPtr != NULL ) {
      startOfDir = startOfDir->siblingPtr;
   }
   return startOfDir;
}

//make directory
void mkdir(char pathName[]){

   char* baseName = (char*) malloc(64);
   char* dirName = (char*) malloc(64*63);
   strcpy(baseName, "");
   strcpy(dirName, "");
   struct NODE* parentDir = splitPath(pathName, baseName, dirName);

   if ( parentDir->childPtr == NULL ) {
      // Create as child of parentDir
      struct NODE* finalDir = (struct NODE*) malloc(sizeof(struct NODE));
      finalDir->parentPtr = parentDir;
      finalDir->childPtr = NULL;
      finalDir->siblingPtr = NULL;
      strcpy(finalDir->name, baseName);
      finalDir->fileType = 'D';
      parentDir->childPtr = finalDir;
      printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
      return; // All done!
   }


   // Otherwise, there are other inodes in this directory and we need to get to
   // the end of the directory to then add a file.
   struct NODE* candidateInode = parentDir->childPtr;
   while ( candidateInode->siblingPtr != NULL ) {
      // If inode we are attempting to make exists, quit.
      if ( strcmp(candidateInode->name, baseName) == 0 ) {
         printf("MKDIR ERROR: directory %s already exists\n",
               pathName);
         return;
      }
      candidateInode = candidateInode->siblingPtr;
   }

   // Okay, it doesn't exist, and we are at the sibling that we need to make a
   // child of.
   struct NODE* finalDir = (struct NODE*) malloc(sizeof(struct NODE));
   finalDir->parentPtr = parentDir;
   finalDir->childPtr = NULL;
   finalDir->siblingPtr = NULL;
   strcpy(finalDir->name, baseName);
   finalDir->fileType = 'D';
   candidateInode->siblingPtr = finalDir;
   printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
   return; // All done!
}

// Helper function to split a path string by '/'
int tokenizePath(char* pathName, char** tokens) {
    int count = 0;
    int j = 0;
    int start = (pathName[0] == '/') ? 1 : 0;  // Skip leading '/'
    
    for (int i = start; pathName[i] != '\0'; i++) {
        if (pathName[i] == '/') {
            if (j > 0) {  // Only add non-empty tokens
                tokens[count][j] = '\0';
                count++;
                j = 0;
            }
        } else {
            tokens[count][j] = pathName[i];
            j++;
        }
    }
    
    // Add the last token if there is one
    if (j > 0) {
        tokens[count][j] = '\0';
        count++;
    }
    
    return count;
}

// Main splitPath function
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Handle NULL or empty path
    if (pathName == NULL || pathName[0] == '\0') {
        strcpy(baseName, "");
        strcpy(dirName, ".");
        return cwd;
    }
    
    bool isAbsolute = (pathName[0] == '/');
    
    // Allocate memory for tokens
    char** tokens = (char**) malloc(64 * sizeof(char*));
    for (int i = 0; i < 64; i++) {
        tokens[i] = (char*) malloc(64);
    }
    
    // Tokenize the path
    int tokenCount = tokenizePath(pathName, tokens);
    
    // Handle special case: just "/"
    if (isAbsolute && tokenCount == 0) {
        strcpy(baseName, "/");
        strcpy(dirName, "/");
        for (int i = 0; i < 64; i++) {
            free(tokens[i]);
        }
        free(tokens);
        return root;
    }
    
    // Set baseName (last token)
    if (tokenCount > 0) {
        strcpy(baseName, tokens[tokenCount - 1]);
    } else {
        strcpy(baseName, "");
    }
    
    // Build dirName (all tokens except the last)
    if (tokenCount <= 1) {
        // No directory path, just a basename
        if (isAbsolute) {
            strcpy(dirName, "/");
        } else {
            strcpy(dirName, ".");
        }
    } else {
        // Build the directory path
        if (isAbsolute) {
            strcpy(dirName, "/");
        } else {
            strcpy(dirName, "");
        }
        
        for (int i = 0; i < tokenCount - 1; i++) {
            if (i > 0 || isAbsolute) {
                strcat(dirName, "/");
            }
            strcat(dirName, tokens[i]);
        }
    }
    
    // Traverse the tree to find the parent directory node
    struct NODE* node = isAbsolute ? root : cwd;
    
    // Navigate to the parent directory (all tokens except last)
    for (int i = 0; i < tokenCount - 1; i++) {
        // Look for the directory in children
        if (node->childPtr == NULL) {
            // Path doesn't exist
            for (int j = 0; j < 64; j++) {
                free(tokens[j]);
            }
            free(tokens);
            return NULL;
        }
        
        node = node->childPtr;
        
        // Search through siblings for matching name
        while (node != NULL && strcmp(node->name, tokens[i]) != 0) {
            node = node->siblingPtr;
        }
        
        if (node == NULL) {
            // Directory not found in path
            for (int j = 0; j < 64; j++) {
                free(tokens[j]);
            }
            free(tokens);
            return NULL;
        }
    }
    
    // Clean up
    for (int i = 0; i < 64; i++) {
        free(tokens[i]);
    }
    free(tokens);
    
    // Return the parent directory node
    return node;
}
