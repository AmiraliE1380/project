#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <winsock2.h>
#include <math.h>
#include "cJSON.h"
#include "cJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

struct online_users{//saves the name and token of online users
    char name[50];
    char token[21];
    int channel_num;
}online_user[100];

struct channels{
    char channels_name[50];
}Channel[1000];

int num_online = 0, num_channel = 0;

int server_socket, client_socket;
struct sockaddr_in server, client;


void initialzie_Users_channel_num(void){
    for(int i = 0; i < 100; i++)
        online_user[i].channel_num = -1;
}

void socket_creation(){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
}

void binding(){
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0){
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");
}

void listening(){
    if ((listen(server_socket, 5)) != 0){
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
}

void accepting(){
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0){
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");

}


// Driver function
int main()
{

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    initialzie_Users_channel_num();//at first no user is in a channel, so its channel_num must be -1

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    // Create and verify socket

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify

    // Function for chatting between client and server


    char buffer[10000];
    int n;
    while (true)
    {

        socket_creation();
        binding();
        listening();
        accepting();



        memset(buffer, 0, sizeof(buffer));

        // Read the message from client and copy it to buffer
        recv(client_socket, buffer, sizeof(buffer), 0);


        // Print buffer which contains the client message
        printf("From client: %s\n", buffer);

        if(buffer[0] == 'r'  && buffer[2] == 'g'){//register
            char username[MAX], password[MAX], users_address[MAX];
            sscanf(buffer, "register %s %s", username, password);
            username[strlen(username) - 1] = '\0';//taking out the "," character

            FILE *fptr, *fptr2; //for recording users
            sprintf(users_address, "username_%s.txt", username);

            if((fptr = fopen(users_address, "r")) == NULL){//in this case there is no userwith this name

                cJSON * ptr;
                ptr = cJSON_CreateObject();
                memset(buffer, 0, sizeof(buffer));

                cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(""));
                strcpy(buffer,cJSON_PrintUnformatted(ptr));
                send(client_socket, buffer, sizeof(buffer), 0);


                memset(buffer, 0, sizeof(buffer));
                cJSON * ptr3;
                ptr3 = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr3,"username",cJSON_CreateString(username));
                cJSON_AddItemToObject(ptr3,"password",cJSON_CreateString(password));
                strcpy(buffer,cJSON_PrintUnformatted(ptr3));


                fptr2 = fopen(users_address, "w");
                //printf("%s", buffer);
                fprintf(fptr2, "%s", buffer);
                //puts("hi2");
                //fprintf(fptr,"%s_%s", username, password);//recording username and password
                //puts("hi3");
                fclose(fptr);// this fclose doesn't work take a look at it!!!!!!!!!!!!!!!!!!!!!
                //puts("hi4");
                fclose(fptr2);
                //puts("hi5");
                puts("User successfully registered.");
            }else{//username already token

                cJSON * ptr;
                ptr = cJSON_CreateObject();
                memset(buffer, 0, sizeof(buffer));

                cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(ptr,"content",cJSON_CreateString("Username already exists."));
                strcpy(buffer,cJSON_PrintUnformatted(ptr));
                send(client_socket, buffer, sizeof(buffer), 0);
                fclose(fptr);

                puts("Username already exists");// this part works good
            }

        }


        if(buffer[0] == 'l' && buffer[3] == 'i'){//login
            char username[MAX], password[MAX], users_address[MAX], file_reader[1000];
            sscanf(buffer, "login %s %s", username, password);
            username[strlen(username) - 1] = '\0';//taking out the "," character

            FILE *fptr, *fptr2; //for finding username between files
            sprintf(users_address, "username_%s.txt", username);
            if((fptr = fopen(users_address, "r")) == NULL){
                puts("user doesn't exist.");

                memset(buffer, 0, sizeof(buffer));
                cJSON * ptr;
                ptr = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(ptr,"content",cJSON_CreateString("There is no user with this name!"));
                strcpy(buffer,cJSON_PrintUnformatted(ptr));
                send(client_socket, buffer, sizeof(buffer), 0);

            }else{

                fgets(file_reader, 1000, fptr);//copying content of file to the array
                cJSON * parse = cJSON_Parse(file_reader);

                if(strcmp(cJSON_GetObjectItem(parse,"password")->valuestring,password)==0){

                    srand(time(NULL));
                    for(int i = 0; i < 20; i++)
                        online_user[num_online].token[i] = (rand() % 10) + '0';
                    online_user[num_online].token[20] = '\0';

                    for(int i = 0; i < strlen(username); i++)
                        online_user[num_online].name[i] = username[i];
                    online_user[num_online].channel_num = -1;

                    memset(buffer, 0, sizeof(buffer));
                    cJSON * ptr;
                    ptr = cJSON_CreateObject();
                    cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("AuthToken"));
                    cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(online_user[num_online].token));
                    strcpy(buffer,cJSON_PrintUnformatted(ptr));
                    send(client_socket, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));

                     //printf("online_user.token = %s\nonline_user.name = %s\nonline_user[i].channel_num = %d\n", online_user[num_online].token, online_user[num_online].name, online_user[num_online].channel_num);
                    //recording data in struct, so that it can be used when working with channels

                    num_online++;
                }else{
                    memset(buffer, 0, sizeof(buffer));
                    cJSON * ptr;
                    ptr = cJSON_CreateObject();
                    cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(ptr,"content",cJSON_CreateString("Wrong password."));
                    strcpy(buffer,cJSON_PrintUnformatted(ptr));
                    send(client_socket, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                }
            }

        }


        if(buffer[0] == 'c' && buffer[7] == 'c'){//create channel

            char channel_name[MAX], temp_token[21];
            sscanf(buffer, "create channel %s %s", channel_name, temp_token);
            channel_name[strlen(channel_name) - 1] = '\0';//elimination the ","

            FILE *fptr;
            char channel_address[MAX];
            sprintf(channel_address, "channel_%s.txt", channel_name);
            if((fptr = fopen(channel_address, "r")) == NULL){//channel with this name doesn't exist, creating channel successfully

                fclose(fptr);
                FILE *fptr2;
                fptr2 = fopen(channel_address, "w");
                memset(buffer, 0, sizeof(buffer));
                cJSON *ptr1, *ptr2, *ptr3;
                //creating the text in the channel's file
                ptr1 = cJSON_CreateObject();
                ptr2 = cJSON_CreateArray();
                cJSON_AddItemToObject(ptr1, "type", cJSON_CreateString(""));
                cJSON_AddItemToObject(ptr1, "content", ptr2);

                int i = 0;
                for(; i < 1000; i++)//finding user by token
                    if(strcmp(temp_token, online_user[i].token) == 0)
                        break;

                //adding the message, declaring that the channel is made by the user
                char declare_creation[MAX];
                sprintf(declare_creation, "Channel is created by %s", online_user[i].name);

                cJSON_AddItemToArray(ptr2, ptr3 = cJSON_CreateObject());
                cJSON_AddItemToObject(ptr3, "sender", cJSON_CreateString("server"));
                cJSON_AddItemToObject(ptr3, "content", cJSON_CreateString(declare_creation));
                strcpy(buffer,cJSON_PrintUnformatted(ptr1));

                fprintf(fptr2,"%s", buffer);
                //printf("buffer printed: %s\n", buffer);//test
                fclose(fptr2);
                memset(buffer, 0, sizeof(buffer));

                cJSON * ptr4;
                ptr4 = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr4,"type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(ptr4,"content",cJSON_CreateString(""));
                strcpy(buffer,cJSON_PrintUnformatted(ptr4));
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                puts("Channel successfully created.");

                strcpy(Channel[num_channel].channels_name,channel_name);// recording channels name in the struct
                num_channel++;

            }else{//in case channel with this name exists...
                fclose(fptr);
                puts("Channel with this name already exists.");
                memset(buffer, 0, sizeof(buffer));
                cJSON * ptr3;
                ptr3 = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr3,"type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(ptr3,"content",cJSON_CreateString("Channel with this name already exists."));
                strcpy(buffer,cJSON_PrintUnformatted(ptr3));
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
            }
        }


        if(buffer[0] == 'j'){//joining channel
            char channel_name[MAX], temp_token[21], channel_address[MAX + 20];//using temp_token to find user

            sscanf(buffer, "join channel %s %s", channel_name, temp_token);
            channel_name[strlen(channel_name) - 1] = '\0';//elimination the ","


            int i = 0;//i is the number of the user between online users
            for(; i < 100; i++)
                if(strcmp(online_user[i].token, temp_token) == 0)
                    break;



            sprintf(channel_address, "channel_%s.txt", channel_name);

            //printf("channel_address = %s\nchannel_name = %s\n", channel_address, channel_name);
            FILE *fptr;
            if((fptr = fopen(channel_address, "r")) == NULL){
                //in case there is no channel with this name
                fclose(fptr);

                //puts("1");
                memset(buffer, 0, sizeof(buffer));
                cJSON * ptr;
                ptr = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(ptr,"content",cJSON_CreateString("Channel not found."));
                strcpy(buffer,cJSON_PrintUnformatted(ptr));
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                puts("Channel with this name doesn't exist.");

            }else{
                //channel with this name found


                //puts("2");
                memset(buffer, 0, sizeof(buffer));
                cJSON * ptr;
                ptr = cJSON_CreateObject();
                cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(""));
                strcpy(buffer,cJSON_PrintUnformatted(ptr));
                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                puts("Successfully joined channel.");


                //int this part I am trying to save the number of the channel in the user's data
                // by finding the channel and the user using the name of the channel and the token of the user
                int i = 0;//finding the channel
                for(; i < 1000; i++)
                    if(strcmp(channel_name, Channel[i].channels_name) == 0)
                        break;

                int j = 0;//finding the user
                for(; j < 100; j++)
                    if(strcmp(temp_token, online_user[j].token) == 0)
                        break;

                online_user[j].channel_num = i;//saving the number of channel in the struct of the user

                sprintf(buffer, "%s joined channel.", online_user[j].name);
                char save[10000], writing_file[10000];//trying to save the content of the file
                fgets(save,10000,fptr);
                printf("saved data from file:%s\n", save);
                fclose(fptr);


                FILE *fptr2;//for writing in channel
                fptr2 = fopen(channel_address, "w");

                cJSON *ptr1, *ptr2, *ptr3;
                ptr1 = cJSON_Parse(save);
                ptr2 = cJSON_GetObjectItem(ptr1,"content");
                cJSON_AddItemToArray(ptr2, ptr3 = cJSON_CreateObject());
                cJSON_AddItemToObject(ptr3, "sender", cJSON_CreateString("server"));
                cJSON_AddItemToObject(ptr3, "content", cJSON_CreateString(buffer));

                strcpy(writing_file,cJSON_PrintUnformatted(ptr1));
                fprintf(fptr2,"%s", writing_file);//printing new string to file
                fclose(fptr2);

                //printf("online_user.token = %s\nonline_user.name = %s\nonline_user[i].channel_num = %d\n", online_user[j].token, online_user[j].name, online_user[j].channel_num);
            }
        }


        if(strncmp(buffer,"send",strlen("send"))==0){//sending message
            char message[10000], temp_token1[21];
            //printf("%d\n", strlen(buffer));
            //memset(temp_token, 0, sizeof(temp_token));


            for(int i = 0; i < 21; i++)//saving token in the array
                temp_token1[i] = buffer[strlen(buffer) - 21 + i];
            for(int i = 21; i < 41; i++)//using a way to debug
                temp_token1[i] = '\0';
            memset(message, 0, sizeof(message));
            for(int i = 0; buffer[i + 5] != ',' || (buffer[i + 5 + 2] < '0' || buffer[i + 5 + 2] > '9'); i++)
                message[i] = buffer[i + 5];//because "send " has five characters

            int i = 0;//for finding user
            temp_token1[20] = NULL;
            for(; i < 100; i++)
                if(strcmp(temp_token1, online_user[i].token) == 0)
                    break;

            int ChannelNumber = online_user[i].channel_num;//channel found
            char channel_address[MAX];
            sprintf(channel_address, "channel_%s.txt", Channel[ChannelNumber].channels_name);//making a string containing the address of the file of the channel

            FILE * fptr;
            fptr = fopen(channel_address, "r");
            char content[10000];//used to save the current content of the file
            fgets(content,10000,fptr);
            fclose(fptr);
            //printf("current content of file: %s\n", content);//test

            cJSON *ptr1, *ptr2, *ptr3;
            ptr1 = cJSON_Parse(content);
            ptr2 = cJSON_GetObjectItem(ptr1, "content");
            cJSON_AddItemToArray(ptr2, ptr3 = cJSON_CreateObject());
            cJSON_AddItemToObject(ptr3, "sender", cJSON_CreateString(online_user[i].name));
            cJSON_AddItemToObject(ptr3, "content", cJSON_CreateString(message));
            char save[10000];//used to save data in the file
            strcpy(save,cJSON_PrintUnformatted(ptr1));


            FILE *fptr2;
            fptr2 = fopen(channel_address, "w");
            fprintf(fptr2, "%s", save);//saving data in the file
            fclose(fptr2);


            memset(buffer, 0, sizeof(buffer));
            cJSON * ptr;//tell user that message successfully sent.
            ptr = cJSON_CreateObject();
            cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Successful"));
            cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(""));
            strcpy(buffer,cJSON_PrintUnformatted(ptr));
            send(client_socket, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            puts("message successfully sent.");
        }


        if(buffer[0] == 'c' && buffer[8] == 'm'){//showing channel members
            char temp_token[21];//it is needed to find the user and the channel he is in.
            sscanf(buffer, "channel members %s", temp_token);

            int i = 0;//the number of the online user
            for(; i < 100; i++)//searching for channel
                if(strcmp(temp_token, online_user[i].token) == 0)
                    break;

            cJSON *ptr, *ptr2;
            ptr = cJSON_CreateObject();
            ptr2 = cJSON_CreateArray();
            cJSON_AddItemToObject(ptr, "type", cJSON_CreateString("list"));
            cJSON_AddItemToObject(ptr, "content", ptr2);

            int ChannelNumber = online_user[i].channel_num; //saving the number of the channel
            //printf("ChannelNumber = %d\n", ChannelNumber);
            for(int j = 0; j < 100; j++)//creating the array of usernames
                if(online_user[j].channel_num == ChannelNumber)
                    cJSON_AddItemToObject(ptr2, "content", cJSON_CreateString(online_user[j].name));

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer,cJSON_PrintUnformatted(ptr));
            send(client_socket, buffer, sizeof(buffer), 0);

        }


        if(buffer[0] == 'r' && buffer[3] == 'r'){//refresh
            char temp_token[21];//it is needed to find the user and the channel he is in.
            sscanf(buffer, "refresh %s", temp_token);

            int i = 0;//used to find the user
            for(; i < 100; i++)
                if(strcmp(online_user[i].token, temp_token) == 0)
                    break;

            int j = online_user[i].channel_num;// j is the number of the channel

            char user_address[MAX];//address of the file that contains user's data
            sprintf(user_address, "username_%s.txt", online_user[i].name);
            //puts("1");

            FILE * fptr;
            fptr = fopen(user_address, "r");
            char content[10000];//content of file will be copied here
            fgets(content, 10000, fptr);//copying content of file to the array
            //puts("2");

            int k1, k2;
            for(k1 = 1; content[k1] != '}'; k1++);//passing json string
            k2 = k1;

            int kx = k1, kz = 0;
            for(; kx < strlen(content); kx++){
                int ky = 0;
                for(; ky < strlen(Channel[j].channels_name); ky++)
                    if(content[kx + ky] != Channel[j].channels_name[ky])
                        break;

                if(ky == strlen(Channel[j].channels_name))
                    kz = 1;
            }


            if(content[k2 + 1] == NULL || kz == 0){//in case user never refreshed any channel
                //puts("2");
                fclose(fptr);
                //puts("2.1");

                char adding_string_user[10000];//used to save the unseen messages in the channel
                FILE *fptr2;
                //puts("2.2");
                char channel_address[MAX];
                sprintf(channel_address,"channel_%s.txt", Channel[j].channels_name);
                fptr2 = fopen(channel_address, "r");
                //recv(client_socket, send_recv, sizeof(send_recv), 0);
                //puts("2.3");

                //take a look at this part:
                memset(buffer, 0, sizeof(buffer));
                fgets(buffer, 10000, fptr2);
                fscanf(fptr2, "%s", buffer);
                printf("1:%s\n", buffer);
                //puts("2.4");
                cJSON* ptr = cJSON_Parse(buffer);
                //puts("2.5");
                //memset(buffer, 0, sizeof(buffer));
                printf("content of channel, the parts that will show up after refresh are: %s\n", cJSON_Print(cJSON_GetObjectItem(ptr , "content")));
                //puts("2.65");
                //strcpy(buffer, cJSON_Print(cJSON_GetObjectItem(ptr , "content")));
                //puts("2.6");


                FILE *fptr3;
                fptr3 = fopen(user_address, "a");
                fprintf(fptr3," %s %d",  Channel[j].channels_name, strlen(buffer));//recording the amount of characters seen by user after refresh in the file
                fclose(fptr3);
                //puts("4");

                send(client_socket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
            }else{

                for(; k1 < strlen(content); k1++){
                    int k3 = 0;
                    for(; k3 < strlen(Channel[j].channels_name); k3++)
                        if(content[k1 + k3] != Channel[j].channels_name[k3])
                            break;

                    if(k3 == strlen(Channel[j].channels_name))
                        break;
                }

                for(; content[k1] != ' '; k1++);//to reach to the end of the name of the channel
                k1++;

                char number[10];
                int temp = 0;
                for(; content[k1] != ' '; k1++)//saving the number recorded, the number represents the amount of characters seen by user from the channel's file
                    number[temp++] = content[k1];

                char *stringPtr;
                int num = strtod(number, &stringPtr);//saving the number


                fclose(fptr);//closing users file
                FILE *fptr2;
                char channel_address[MAX];
                sprintf(channel_address,"channel_%s.txt", Channel[j].channels_name);
                fptr2 = fopen(channel_address, "r");//opening channel's file

                memset(buffer, 0, sizeof(buffer));
                fgets(buffer, 10000, fptr2);
                fscanf(fptr2, "%s", buffer);
                printf("1:%s\n", buffer);
                puts("hi2");
                int size_buffer = strlen(buffer);//size of data it channel's file

                char channel_save[size_buffer - num + 2];//saves channel's unread messages

                //reaching the end of saved data:
                printf("%d\n", num);
                int k4 = 2;
                for(; k4 < size_buffer - num ; k4++){
                    channel_save[k4] = buffer[k4 + num - 1];
                    //puts("!");
                }
                channel_save[0] = '[';
                channel_save[1] = '{';
                //channel_save[k4] = ']';

                for(int i = strlen(channel_save) - 1; channel_save[i] != ']'; i--)
                    channel_save[i] = '\0';
                for(int i = strlen(channel_save) - 1; i > 1; i--)
                    channel_save[i + 1] = channel_save[i];
                channel_save[2] = '"';
                printf("k4 = %d\nstrlen(channel_save) = %d\n", k4, strlen(channel_save));

                printf("channel_save = %s\nbuffer = %s\n", channel_save, buffer);

                char c = '"', string[MAX];
                sprintf(string, "{%ctype%c:%c%c,%ccontent%c:", c, c, c, c, c, c);//making a string to send to client
                sprintf(buffer, "%s%s}", string, channel_save);//buffer is ready to send
                printf("\n\nfinal buffer for sending to client:%s\n\n", buffer);
                send(client_socket, buffer, sizeof(buffer), 0);

                cJSON* ptr = cJSON_Parse(buffer);
                printf("content of channel, the parts that will show up after refresh are: %s\n", cJSON_Print(cJSON_GetObjectItem(ptr , "content")));

                memset(buffer, 0, sizeof(buffer));
                fclose(fptr2);
                FILE *fptr3, *fptr4;//for updating the user's file by writing how many characters have been seen
                fptr3 = fopen(user_address, "r");
                fgets(buffer, 10000, fptr3);
                printf("buffer after user file read, before it has changed:%s\n", buffer);
                fclose(fptr3);
                for(; k2 < 10000; k2++){
                    int i = 0;
                    for(; i < strlen(Channel[j].channels_name); i++)//you are here... save new information in file of user...
                        if(buffer[i + k2] != Channel[j].channels_name[i])
                            break;
                    if(i == strlen(Channel[j].channels_name))
                       break;
                }

                printf("k2 = %d, buffer[k2] = %d\n", k2, buffer[k2]);

                for(;buffer[k2] != ' '; k2++);//to reach to the end of the name of the channel
                k2++;

                printf("k2 = %d, buffer[k2] = %c\n", k2, buffer[k2]);

                int temp4;
                for(int i = 0; buffer[k2 + i] != ' ' && buffer[k2 + i] != NULL; i++){//eliminating the number of characters read in the past
                    buffer[k2 + i] = ' ';
                    temp4 = i;
                }
                printf("temp4 = %d\n", temp4);

                //char Number[10];//for saving the updated number of characters read
                size_buffer;
                int n = 0, temp_size = size_buffer;
                for(; ; n++)
                    if(temp_size / 10 == 0)
                        break;
                    else
                        temp_size /= 10;

                printf("n = %d\n", n);

                for(; n != -1; n--){

                    int div = 1;
                    for(int i = 0; i < n; i++)
                        div *= 10;
                    buffer[k2++] = '0' + size_buffer / div;

                    printf("%d    %d    %d   %c\n",size_buffer, div, size_buffer / div, buffer[k2 - 1]);
                    size_buffer %= div;
                }

                printf("buffer after all changes = %s\n", buffer);
                fptr4 = fopen(user_address, "w");
                fprintf(fptr4, "%s", buffer);
                fclose(fptr4);
                memset(buffer, 0, sizeof(buffer));

            }
        }


        if(buffer[0] == 'l' && buffer[1] == 'e'){//leave channel
            char temp_token[21];//it is needed to find the user and the channel he is in.
            sscanf(buffer, "leave %s", temp_token);

            int channel_number, i = 0;//the channel number will be used to find the channel to write the declaration of leaving
            for(; i < 100; i++)
                if(strcmp(online_user[i].token, temp_token) == 0){
                    channel_number = online_user[i].channel_num;
                    online_user[i].channel_num = -1;//-1 means that user is not in any channel
                    break;//so that the number of the user is saved in i
                }

            //telling user that he left the channel successfully:
            memset(buffer, 0, sizeof(buffer));
            cJSON * ptr;
            ptr = cJSON_CreateObject();
            cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Successful"));
            cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(""));
            strcpy(buffer,cJSON_PrintUnformatted(ptr));
            send(client_socket, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            puts("User left channel.");

            //recording in file that user left channel:
            char channel_address[MAX];
            sprintf(channel_address, "channel_%s.txt", Channel[channel_number].channels_name);//creating the address of the file having records of the channel

            //saving the current content of the file:
            FILE * fptr;
            fptr = fopen(channel_address, "r");
            char content[10000];//used to save the current content of the file
            fgets(content,10000,fptr);
            fclose(fptr);

            cJSON *ptr1, *ptr2, *ptr3;
            ptr1 = cJSON_Parse(content);
            ptr2 = cJSON_GetObjectItem(ptr1, "content");
            cJSON_AddItemToArray(ptr2, ptr3 = cJSON_CreateObject());
            cJSON_AddItemToObject(ptr3, "sender", cJSON_CreateString("server"));
            sprintf(buffer, "%s left the channel.", online_user[i].name);//making the string that declares leaving
            cJSON_AddItemToObject(ptr3, "content", cJSON_CreateString(buffer));
            char save[10000];//used to save data in the file
            strcpy(save,cJSON_PrintUnformatted(ptr1));


            FILE *fptr2;
            fptr2 = fopen(channel_address, "w");
            fprintf(fptr2, "%s", save);//saving data in the file
            fclose(fptr2);
            memset(buffer, 0, sizeof(buffer));

        }

        if(buffer[0] == 's' && buffer[7] == 'm'){//searching members in channel
            //puts("hi");
            char temp_token[21], search_name[MAX];
            sscanf(buffer, "search member %s %s", search_name, temp_token);

            int k = 0;//founds the user with the given token
            for(; k < 100; k++)
                if(strcmp(online_user[k].token, temp_token) == 0)
                    break;
            int channel_number = online_user[k].channel_num;

            int j = 0; //j is the variable that if its amount becomes 1, it means that the user exists in channel
            for(int i = 0; i < 100; i++)
                if(online_user[i].channel_num == channel_number)
                    if(strcmp(online_user[i].name, search_name) == 0){
                        j = 1;
                        break;
                    }

            memset(buffer, 0, sizeof(buffer));
            if(j == 1)//if a user with this name exists in channel
                strcpy(buffer, "true");
            else//if a user with this name doesn't exists in channel
                strcpy(buffer, "false");

            //printf("buffer = %s\n", buffer);
            send(client_socket, buffer, sizeof(buffer), 0);
        }

        if(buffer[0] == 's' && buffer[7] == 'w'){// searching for words in channel
            char temp_token[21], search_word[MAX];
            sscanf(buffer, "search word %s %s", search_word, temp_token);


            int k = 0;//founds the user with the given token
            for(; k < 100; k++)
                if(strcmp(online_user[k].token, temp_token) == 0)
                    break;
            int channel_number = online_user[k].channel_num;
            char channel_name[MAX], channel_address[MAX];
            strcpy(channel_name, Channel[channel_number].channels_name);

            sprintf(channel_address, "channel_%s.txt", channel_name);

            FILE *fptr;
            fptr = fopen(channel_address, "r");
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, 10000, fptr);//copying the content of the channel in to the string
            fclose(fptr);

            printf("copied from file = %s\n", buffer);

            cJSON* eb = cJSON_Parse(buffer);
            printf("%s\n", cJSON_Print(cJSON_GetObjectItem(eb , "content")));
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, cJSON_Print(cJSON_GetObjectItem(eb , "content")));

            //GetObjectItem(buf,"type",result);

            //cJSON *rec= cJSON_Parse(buffer);
            //cJSON *type = cJSON_GetObjectItemCaseSensitive(rec, "content");
            //memset(buffer, 0, sizeof(buffer));
            //strcpy(buffer, type);
            //printf("1:\n%s\n", buffer);

            //printf("10:\n%s\n", cJSON_GetObjectItemCaseSensitive(eb, "content"));


            /*cJSON *rec= cJSON_Parse(buffer);
            cJSON *array= NULL;
            cJSON *content = cJSON_GetObjectItemCaseSensitive(rec, "content");

            cJSON_ArrayForEach(array, content){

                cJSON *sender = cJSON_GetObjectItemCaseSensitive(array, "sender");
                cJSON *content1 = cJSON_GetObjectItemCaseSensitive(array, "message");
                printf("\n sender : %s \n content: %s \n ***",sender->valuestring,content1->valuestring);

            }*/


            //cJSON*rec = cJSON_Parse(buffer);
            //cJSON *content = cJSON_GetObjectItemCaseSensitive(rec, "type");
            //printf("\n result is %s\n", content->valuestring);
            //printf("buffer after parsing = %s\n", cJSON_Print(cJSON_GetObjectItem(ptr , "content")));
            memset(buffer, 0, sizeof(buffer));
            //strcpy(buffer, cJSON_Print(cJSON_GetObjectItem(ptr , "content")));

           // send(client_socket, "end_of_search", sizeof("end_of_search"), 0);





           /* memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"join channel %s, %s\n",channelname,token);
                        server_socket = socket(AF_INET, SOCK_STREAM, 0);
                        connect(server_socket, (SA*)&servaddr, sizeof(servaddr));
                        send(server_socket, buffer, sizeof(buffer), 0);
                        memset(buffer, 0, sizeof(buffer));
                        recv(server_socket, buffer, sizeof(buffer), 0);
                        cJSON *rec= cJSON_Parse(buffer);
                        cJSON *content = cJSON_GetObjectItemCaseSensitive(rec, "type");
                        printf("\n result is %s\n", content->valuestring);
                        closesocket(server_socket);
                        while(1){
                                send_menu:
                                printf("\n you have 4 options here:\n 1:send massage\n 2: refresh\n 3: channel members\n 4: leave channel\n 5: go to channels menu \n 6: go to send menu\n");
                                int resp;
                                scanf("%d",&resp);
                                if(resp==1){
                                    char sendmas[100];
                                    scanf("%s",sendmas);
                                    memset(buffer, 0, sizeof(buffer));
                                    sprintf(buffer,"send %s, %s\n",sendmas,token);
                                    server_socket = socket(AF_INET, SOCK_STREAM, 0);
                                    connect(server_socket, (SA*)&servaddr, sizeof(servaddr));
                                    send(server_socket, buffer, sizeof(buffer), 0);
                                    memset(buffer, 0, sizeof(buffer));
                                    recv(server_socket, buffer, sizeof(buffer), 0);
                                    cJSON *rec= cJSON_Parse(buffer);
                                    cJSON *content = cJSON_GetObjectItemCaseSensitive(rec, "type");
                                    printf("\n result is %s\n", content->valuestring);
                                    closesocket(server_socket);
                                }
                                else if(resp==2){
                                    memset(buffer, 0, sizeof(buffer));
                                    sprintf(buffer,"refresh %s\n",token);
                                    server_socket = socket(AF_INET, SOCK_STREAM, 0);
                                    connect(server_socket, (SA*)&servaddr, sizeof(servaddr));
                                    send(server_socket, buffer, sizeof(buffer), 0);
                                    memset(buffer, 0, sizeof(buffer));
                                    recv(server_socket, buffer, sizeof(buffer), 0);
                                    //printf("%s",buffer);
                                    cJSON *rec= cJSON_Parse(buffer);
                                    cJSON *array= NULL;
                                    cJSON *content = cJSON_GetObjectItemCaseSensitive(rec, "content");
                                    cJSON_ArrayForEach(array, content)
                                    {
                                        cJSON *sender = cJSON_GetObjectItemCaseSensitive(array, "sender");
                                        cJSON *content = cJSON_GetObjectItemCaseSensitive(array, "message");
                                        printf("\n sender : %s \n content: %s \n ***",sender->valuestring,content->valuestring);
                                    }
            */





            /*for(int i = 0; buffer[i] != NULL; i++){
                char string[1000];
                if(buffer[i] == '{'){
                    int j = i;
                    //string[0] = '{';
                    for(;buffer[i - 1] != '}'; i++)
                        string[i - j] = buffer[i];

                    cJSON* eb = cJSON_Parse(string);
                    if(strcmp("server", cJSON_Print(cJSON_GetObjectItem(eb , "sender"))) != 0){
                        char string2[1000];//may contain the message that has the word in it
                        strcpy(string2, cJSON_Print(cJSON_GetObjectItem(eb , "content")));

                        for(int k = 0; k < strlen(string2); k++){
                            int k2 = 0;
                            for(; k2 < strlen(search_word); k2++)
                                if(string2[k2] != search_word[k2])
                                    break;

                            if(k2 == strlen(search_word) - 1){//in case there is a word identical the the word we are searching for in the channel
                                send(client_socket, string2, sizeof(string2), 0);
                            }
                        }

                    }



                }



            }*/

            //send(client_socket, "end_of_search", sizeof("end_of_search"), 0);


        }


        if(buffer[0] == 'l' && buffer[3] == 'o'){//logout
            char temp_token[21];//need it to find the client which is know connected by socket
            sscanf(buffer, "logout %s", temp_token);

            int num_client;// the number of the client between online users
            for(int i = 0; i < 100; i++){
                if(strcmp(temp_token, online_user[i].token) == 0){
                    //printf("in = %s\n", online_user[i].token);
                    num_client = i;
                    break;
                }
                //printf("%s %s\n",temp_token, online_user[i].token);
            }

            //for test:
            //printf("online_user.token = %s\nonline_user.name = %s\nonline_user[i].channel_num = %d\n", online_user[num_client].token, online_user[num_client].name, online_user[num_client].channel_num);

            for(int i = num_client; i < num_online; i++){//for replacing the memory occupied by the user who left.
                for(int j = 0; j < 21; j++)
                    online_user[i].token[j] = online_user[i + 1].token[j];
                for(int j = 0; j < 50; j++)
                    online_user[i].name[j] = online_user[i + 1].name[j];
                online_user[i].channel_num = online_user[i + 1].channel_num;
            }

            //for test:
            //printf("online_user.token = %s\nonline_user.name = %s\nonline_user[i].channel_num = %d\n", online_user[num_client].token, online_user[num_client].name, online_user[num_client].channel_num);
            num_online--;

            cJSON * ptr;
            ptr = cJSON_CreateObject();
            memset(buffer, 0, sizeof(buffer));

            cJSON_AddItemToObject(ptr,"type",cJSON_CreateString("Successful"));
            cJSON_AddItemToObject(ptr,"content",cJSON_CreateString(""));
            strcpy(buffer,cJSON_PrintUnformatted(ptr));
            send(client_socket, buffer, sizeof(buffer), 0);
        }

        memset(buffer, 0, sizeof(buffer));
        n = 0;

        // Copy server message to the buffer
        //while ((buffer[n++] = getchar()) != '\n')
          //  ;

        // Send the buffer to client
        //send(client_socket, buffer, sizeof(buffer), 0);


        // If the message starts with "exit" then server exits and chat ends
        if (strncmp("exit", buffer, 4) == 0)
        {
            printf("Server stopping...\n");
            return;
        }












        closesocket(server_socket);
/*
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            printf("Socket creation failed...\n");
            exit(0);
        }

        if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0){
            printf("Socket binding failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully bound..\n");

        if ((listen(server_socket, 5)) != 0){
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("Server listening..\n");

        client_socket = accept(server_socket, (SA *)&client, &len);*/
    }



    // Close the socket
    closesocket(server_socket);

}
