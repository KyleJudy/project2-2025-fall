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

   // TO BE IMPLEMENTED
   //
   // YOUR CODE TO REPLACE THE PRINTF FUNCTION BELOW

   char* baseName = (char*) malloc(64);
   char* dirName = (char*) malloc(64*63);
   strcpy(baseName, "");
   strcpy(dirName, "");
   struct NODE* parentDir = splitPath(pathName, baseName, dirName);

   // printf("%s  %s\n", dirName, baseName);
   
   if ( parentDir->childPtr == NULL ) {
      // Create as child of parentDir
      struct NODE* finalDir = (struct NODE*) malloc(sizeof(struct NODE));
      finalDir->parentPtr = parentDir;
      finalDir->childPtr = NULL;
      finalDir->siblingPtr = NULL;
      strcpy(finalDir->name, baseName);
      finalDir->fileType = 'd';
      return; // All done!
   }
   // Otherwise, there are other inodes in this directory and we need to get to
   // the end of the directory to then add a file.
   struct NODE* candidateInode = parentDir->childPtr;
   while ( strcmp(candidateInode->name, baseName) &&
           candidateInode->siblingPtr != NULL ) {
      candidateInode = candidateInode->siblingPtr;
   }
   // Now we are at the thing we need to make, let's check that it doesn't exist
   // yet.
   if ( strcmp(candidateInode->name, baseName) ) {
      printf("mkdir: cannot create directory '%s': File exists\n",
            pathName);
      return;
   }

   // Okay, it doesn't exist, and we are at the sibling that we need to make a
   // child of.

   return;

   size_t scanloc = 0;

   if ( pathName[0] == '/' ) {
      // Do root calculation scanloc++;
   }
   // Do relative calculation
   char *directPathName[64];
   for ( int i = 0; i < 64; i++ ) {
      directPathName[i] = (char*) malloc(64);
   }
   size_t depth0 = 0;
   size_t depth1 = 0;
   while ( pathName[scanloc] != '\0' ) { // Scan until end of string
      while ( pathName[scanloc] != '/' && pathName[scanloc] != '\0' ) {
         directPathName[depth0][depth1] = pathName[scanloc];
         scanloc++;
         depth1++;
      }
      printf("Path: %s\n", directPathName[depth0]);
      depth1 = 0;
      depth0++;
      scanloc++;
   }

   // Now we have the path, let's make our directory!
   struct NODE* start = cwd;
   if ( pathName[0] == '/' ) {
      start = root;
   }


   size_t depth2 = depth0;
   depth0 = 0;
   printf("depth2: %d\n", depth2);

   // I'm not quite sure if I should emulate -p behavior or not, I'm going to
   // assume not.
   while ( depth0 < depth2 - 1 ) { // Use -1 because that is the pointer we want
                                   // that is just before where we exit
      while ( start->siblingPtr != NULL            // While there is a sibling
           && start->name != directPathName[depth0]// and we aren't at the node
                                                   // we want
            ) {
         start = start->siblingPtr; // Change start to another candidate pointer
                                    // in this level of the filesystem
      }
      depth0++;
   }
   // Now we are at the node we wish to create a child of, let's start work on
   // making that child!
   
   //struct NODE* finaldir;
   struct NODE* finaldir = (struct NODE*) malloc(sizeof(struct NODE));
   finaldir->parentPtr = start;
   finaldir->siblingPtr = NULL;
   finaldir->fileType = 'd';
   finaldir->childPtr = NULL;
   strcpy(finaldir->name, directPathName[depth2]);
   if ( start->childPtr != NULL ) { // If there are other inodes in directory
                                    // that is parent of requested dir to create
      start = endOfDir(start->childPtr);
      start->siblingPtr = finaldir;
   } else {
      start->childPtr = finaldir;
   }

   free(finaldir);

   return;
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
