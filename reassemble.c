#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <json-c/json.h>

#define BUFFER_MAX_JSON 1024
#define BUFFER_SIZE 64
#define MAX_TOTAL_FILES 80
#define MAX_STRING_SIZE 100

int main()
{
  int   file;
  int   output;
  char  buffer[BUFFER_SIZE];
  int   read_size;
  char  counter;
  FILE* file_sha256;
  DIR *directory;
  directory = opendir("output");
  char dir[MAX_STRING_SIZE];
  char temp[MAX_STRING_SIZE];
  char files[MAX_TOTAL_FILES][MAX_STRING_SIZE];
  struct dirent* fp;
  FILE *report;
  char buffer_json[BUFFER_MAX_JSON];
  char sha256_json[BUFFER_SIZE];
  struct json_object *parsed_json;
  struct json_object *sha256;
  char buffer_output [BUFFER_SIZE];

  output = open("new", O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IXUSR | S_IWUSR);
  
  if (directory == NULL){
    printf("Error:: Directory not found\n");
    exit(0);
  }
  //Finding the files of all the parts in the output directory
  while ((fp=readdir(directory))!=NULL){
     if(fp->d_type == DT_REG){
        strcpy(files[counter],fp->d_name);
        counter++;
      }
      
  }
  if (counter == 0){
    printf("Error:: No files in the directory\n");
    exit(0);
  }
  //Sort alphabetically all the file names
  for(int i=0;i<counter;i++){
    for(int j=0;j<counter;j++){
      if(strcmp(files[i],files[j])<0){
        strcpy(temp,files[i]);
        strcpy(files[i],files[j]);
        strcpy(files[j],temp);
      }
    }
  }

  //Reading and writting each file to generate the new one
  for(int i=0;i<counter;i++){
      strcpy(dir,"output/");
      strcat(dir,files[i]);
      file = open(dir, O_RDONLY);
         while ((read_size = read(file, buffer, BUFFER_SIZE)) > 0)
           write(output, &buffer, read_size);
         close(file);
  }
  close(output);

  //Getting the sha256 checksum from the report.json
  report = fopen("report.json","r");
  fread(buffer_json,BUFFER_MAX_JSON,1,report);
  fclose(report);

  parsed_json = json_tokener_parse(buffer_json);
  json_object_object_get_ex(parsed_json,"sha256sum",&sha256);
  memcpy(sha256_json,json_object_get_string(sha256),BUFFER_SIZE+1);

  //Getting sha256 checksum from the new file
  file_sha256 = popen("sha256sum new", "r");
  if(fscanf(file_sha256 ,"%64s",buffer_output)!=1){
    printf("Error:: Not file found\n");
    exit(0);
  }
  pclose(file_sha256);
  
  printf("%s\n", buffer_output);
  //Comparing the two sha256 checksum  
  if(memcmp(sha256_json,buffer_output,64)==0)
    printf("File integrity: OK\n");
  else
    printf("File integrity: Error\n");

  return (0);
}
