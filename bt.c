/* 
 * File:   bt.c
 * Author: Hana Abuzeid
 * Dr. Mark Hauschild
 * Created on February, 2021
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h> 
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <getopt.h>

#define QUEUESIZE 2000
#define PATHSIZE  250


//Add path to queue, insert in the left
// It returns 1 if successful, 0 if unable to insert, because it's full
int enqueue(char queue[QUEUESIZE][PATHSIZE],char path[PATHSIZE])
{
    int i;
    
    for(i=QUEUESIZE - 1; i > 0 ; i--)
    {
        if(strcmp(queue[i],"") == 0){
            strcpy(queue[i],path);            
            return 1;
        }
    }
    return 0;
}

//remove path from end of queue, and adjust queue for the empty space
// return 0 if empty. Return 1 if an element was popped off the end
int dequeue(char queue[QUEUESIZE][PATHSIZE],char path[PATHSIZE])
{
    int i=0;
    
    if(strcmp(queue[QUEUESIZE - 1],"") == 0)
        return 0;
    //Save end value to pass back, pop of the end of the queue
    strcpy(path,queue[QUEUESIZE-1]);
    //Shuffle the queue
    for(i=QUEUESIZE - 1 ; i > 1; i--){
        strcpy(queue[i],queue[i-1]);        
    }
    strcpy(queue[0],"");
    return 1;
}

void displayQueue(char queue[QUEUESIZE][PATHSIZE])
{
    int i;
    for(i=0; i < QUEUESIZE; i++){
        if(strcmp(queue[i],"") != 0){
            printf("%d) %s\n",i,queue[i]);
        }
    }
}

int printModes(const char *path)
{
    struct stat ret;
    char modesStr[20];
    int i=0;
    if (stat(path, &ret) == -1) {
        return -1;
    }
    for(i=0;i<9;i++){
        modesStr[i] = '-';
    }  
    
    i=0; 
    
    // User
    if(ret.st_mode & S_IRGRP){
        modesStr[i] = 'r';
    }
    i++;
        if(ret.st_mode & S_IWGRP){
        modesStr[i] = 'w';
    }
    i++;
        if(ret.st_mode & S_IXGRP){
        modesStr[i] = 'x';
    }
    i++;
    
    //Group
    
    if(ret.st_mode & S_IRUSR){
        modesStr[i] = 'r';
    }
    i++;
        if(ret.st_mode & S_IWUSR){
        modesStr[i] = 'w';
    }
    i++;
        if(ret.st_mode & S_IXUSR){
        modesStr[i] = 'x';
    }
    i++;
    
    // Other
    
    if(ret.st_mode & S_IROTH){
        modesStr[i] = 'r';
    }
    i++;
        if(ret.st_mode & S_IWOTH){
        modesStr[i] = 'w';
    }
    i++;
        if(ret.st_mode & S_IXOTH){
        modesStr[i] = 'x';
    }   
    i++;
    modesStr[i] = '\0';
    
    printf("%s", modesStr);
    return 0;
}

void printDate(const char *path)
{
    char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May","Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    struct stat ret;
    
    if (stat(path, &ret) == -1) {
        return;
    }
    
    struct tm tm2 = *localtime(&ret.st_mtime);
    printf(" %s %2d, %4d ", months[tm2.tm_mon], tm2.tm_mday, tm2.tm_year + 1900);
}

// Return 1 if the path passed in is a link
// else return a 0
int isSymlink(const char *path)
{
    struct stat ret;
    
    if (lstat(path, &ret) == -1) {
        return 0;
    }
    
    switch (ret.st_mode & S_IFMT) {             
        case S_IFLNK:  return 1; break;        
        default:        break;
    }
    return 0;
}
  
void printType(const char *path)
{
    struct stat ret;
    
    if (lstat(path, &ret) == -1) {
        return;
    }
    
    switch (ret.st_mode & S_IFMT) {
        case S_IFBLK:  printf("b"); break;
        case S_IFCHR:  printf("c"); break;
        case S_IFDIR:  printf("d"); break;
        case S_IFIFO:  printf("p"); break;
        case S_IFLNK:  printf("l"); break;
        case S_IFREG:  printf("-"); break;
        case S_IFSOCK: printf("s"); break;
        default:       printf("?"); break;
    }
}

void printUID(const char *path)
{
   struct stat ret;
   struct passwd *pwd;   
       
   if (stat(path, &ret) == -1) {
        return;
   }
   
   if((pwd = getpwuid(ret.st_uid)) != NULL)
       printf(" %-8.8s", pwd->pw_name);
   else
        printf(" %6u ", ret.st_uid);
}

void printGID(const char *path)
{   
    struct stat ret;
    struct passwd *pwd;
    
    if (stat(path, &ret) == -1) {
       return;
    }
      
   if((pwd = getpwuid(ret.st_gid)) != NULL)
       printf(" %-8.8s", pwd->pw_name);
   else
        printf(" %6u ", ret.st_gid);
}

void printNumberOfLinks(const char *path)
{ 
    struct stat ret;
    
    if (stat(path, &ret) == -1) {
       return;
    }   
    
    printf(" %4hu ", ret.st_nlink);

}

void printFileSize(const char *path)
{
    struct stat ret;
    unsigned int fileSize;
    
    if (stat(path, &ret) == -1) {
       return;
    }   
  
    if(ret.st_size >= 1000000000){
        fileSize = ret.st_size/1000000000;
        printf(" %4dG ",fileSize);
    }else if(ret.st_size >= 1000000){
        fileSize = ret.st_size/1000000;
        printf(" %4dM ",fileSize);
    }else if (ret.st_size >= 1000){
        fileSize = ret.st_size/1000;
        printf(" %4dK ",fileSize);
    }else{
        fileSize = ret.st_size;
        printf("  %4d ", fileSize);
    }    
}

void searchDirectories(const char *name, int L, int t, int p, int i, int u, int g, int s, int d,char* appName)
{
    DIR *dir;  
    struct dirent *entry;
    char queue[QUEUESIZE][PATHSIZE];
    int idx=0;
    char apath[PATHSIZE];
    char childPath[PATHSIZE];
    int len;
    struct stat ret;    
    char errorBuf[50];
    int symlink;
    int visitSimLink;
    //set the queue to empty
    for(idx=0; idx < QUEUESIZE; idx++){
        strcpy(queue[idx], "");
    }    
  
    if (!(dir = opendir(name))){
        sprintf (errorBuf, "%s: Error", appName);
        perror(errorBuf);
        return;
    }
    if (!(entry = readdir(dir))){       
        printf("Nothing to search.\n");
        return;
    }

    enqueue(queue,(char *)name);
    while(dequeue(queue,apath)){
        //Read in the children directories to the queue
        dir = opendir(apath);
        symlink = isSymlink(apath);        
       
        // Don't visit symlinks when L == 0
        visitSimLink = 1;
        if(L == 0 && symlink == 1){
            visitSimLink = 0;           
        }
        
        if(dir != NULL && visitSimLink){          
            while((entry = readdir(dir)) != 0){ // == 
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                len = snprintf(childPath, sizeof(apath)-1, "%s/%s", apath, entry->d_name);
                childPath[len] = 0;                
                enqueue(queue,childPath);                 
            }  
            closedir(dir); //closedir
        }
    
        
        if(t)
            printType(apath);
        if(p)
            printModes(apath);
        if(i)
            printNumberOfLinks(apath);
        if(u)
            printUID(apath);
        if(g)
            printGID(apath);
        if(s)
            printFileSize(apath);
        if(d)
            printDate(apath);    
        printf(" %s\n",apath);
        // printf("a/n")

    }

}

void printHelp(char *appName)
{
    printf("How to use '%s', a file directory display utility. \n\n",appName);
    
    printf("%s [-h] [-L -d -g -i -p -s -t -u | -l] [dirname]\n\n",appName);
    
    printf("-h for help\n");
    printf("-L to follow symbolic links.\n");
    printf("-d to show the time of the last modificaton.\n");
    printf("-g to print the GID associated with the file.\n");
    printf("-i to print number of links to file in inode table.\n");
    printf("-p to print permissions.\n");
    printf("-s to print the file size.\n");
    printf("-t to print file type information.\n");
    printf("-u to print the UID associated with the file.\n");
    printf("-l is equivalent to the options tpiugs.\n\n");
}

int main(int argc, char** argv) {

    char appName[50];
    char searchPathRoot[200];
    int option;
    int len, j, k;
    int h=0, L=0, t=0, p=0, i=0, u=0, g=0, s=0, d=0, l=0; // Command line switches

    
    // Extract application name from path
    len = strlen(argv[0]);        
    for(j=len-1; j >= 0; j--){       
        if(argv[0][j] == '/' || argv[0][j] == '.' || j == 0){
            if(j != 0)
                j++;
            k = 0;
            while(argv[0][j] != '\0' ){
                appName[k++] = argv[0][j++];
            }
            appName[k] = '\0';
            break;
        }     
    }
    
    while((option = getopt(argc, argv, "hLtpiugsdl")) != -1){
        switch(option){
            case 'h':
                printHelp(appName);
                return (EXIT_SUCCESS);
                break;
            case  'L':
                L = 1;
                break;
            case 't':
                t = 1;
                break;
            case 'p':
                p = 1;
                break;
            case 'i':
                i = 1;
                break;
            case 'u':
                u = 1;
                break;
            case 'g':
                g = 1;
                break;
            case 's':
                s = 1;
                break;
            case 'd':
                d = 1;
                break;
            case 'l':
                l = 1;
                break;
        }
    }
    
    if(l == 1){
        //tpiugs
        t = p = i = u = g = s = 1;
    }
    
    //Default directory
    strcpy(searchPathRoot,".");
    // determine root search path
    
    if(argc > 1 && argv[argc-1][0] != '-'){
        strcpy(searchPathRoot,argv[argc-1]);
    }

    searchDirectories(searchPathRoot, L, t, p, i, u, g, s, d, appName);
    return (EXIT_SUCCESS);
}

