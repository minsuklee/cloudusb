#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "header.h"

int in_file;
int out_file;

int media_init(char *media_path);
int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count);
int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count);
void media_close();

int main(int argc, char *argv[])
{
    FL_FILE *file;

    if (media_init("/Users/lunahc92/Desktop/test.dmg") != 1)
    {
        puts("media_init error");
        return -1;
    }
    
    fl_init();
    
    if (fl_attach_media(media_read, media_write) != FAT_INIT_OK)
    {
        printf("ERROR: Media attach failed\n");
        return 1;
    }
    
    file = fl_fopen("/Dongle.txt", "w");
    if (file)
    {
        // Write some data
        unsigned char data[] = { 1, 2, 3, 4, 5};
        if (fl_fwrite(data, 1, sizeof(data), file) != sizeof(data))
            printf("ERROR: Write file failed\n");
    }
    else
        printf("ERROR: Create file failed\n");
    
    fl_fclose(file);
    
    fl_listdirectory("/");
    
    media_close();
}

int media_init(char *media_path)
{
    if((in_file = open(media_path, O_RDONLY)) < 0)
    {
        perror(media_path);
        return -1;
    }
    
    if((out_file = open(media_path, O_WRONLY)) < 0)
    {
        perror(media_path);
        return -1;
    }
    
    return 1;
}

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    unsigned long i;
    
    for (i=0;i<sector_count;i++)
    {
        lseek(in_file, 512*sector, SEEK_SET);
        
        if(read(in_file, buffer, 512) < 0)
        {
            perror("read");
            return 0;
        }
        
        sector ++;
        buffer += 512;
    }
    
    return 1;
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    unsigned long i;
    
    for (i=0;i<sector_count;i++)
    {
        lseek(out_file, 512*sector, SEEK_SET);
        
        if(write(out_file, buffer, 512) < 0)
        {
            perror("write");
            return 0;
        }
        
        sector ++;
        buffer += 512;
    }
    
    return 1;
}

void media_close(){
    close(out_file);
    close(in_file);
}
