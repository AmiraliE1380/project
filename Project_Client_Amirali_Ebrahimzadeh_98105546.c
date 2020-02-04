#include <stdio.h>
#include "cJSON.h"
#include "cJSON.c"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <winsock2.h>
struct sockaddr_in servaddr;
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
// Function designed for chat between client and server.
void chat(int client_socket)
{
    printf("Account Menu:\n1: Register\n2: Login\n");//
    char message[10000], send_recv[10000], buffer[MAX], username[MAX], password[MAX], channel_name[MAX], token[MAX];
	int n, demand, demand_channel;//






    scanf("%d", &demand);//
    while( demand == 1){//registering can be done many times

        puts("Enter Username");//
        scanf("%s", username);//
        puts("Enter Password");//
        scanf("%s", password);//
        sprintf(buffer, "register %s, %s\n", username, password);//
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("Connection to the server failed...\n");
            exit(0);
        }
        send(client_socket, buffer, sizeof(buffer), 0);//
        memset(buffer, 0, sizeof(buffer));//
        recv(client_socket, buffer, sizeof(buffer), 0);//
        closesocket(client_socket);

        cJSON* eb = cJSON_Parse(buffer);//Ask Javad
        cJSON_GetObjectItem(eb , "type")->valuestring;//Ask Javad
        cJSON_GetObjectItem(eb , "content");//

        system("@cls||clear");//erasing the previous messages

        if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Successful") == 0)
            printf("Successfully registered!\n");
        else
            puts("Sorry, this username already exists, please register again.");
        memset(buffer, 0, sizeof(buffer));
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        printf("Account Menu:\n1: Register\n2: Login\n");//
        scanf("%d", &demand);
    }


    if(demand == 2){

        puts("Enter Username");//
        scanf("%s", username);//
        puts("Enter Password");//
        scanf("%s", password);//
        sprintf(buffer, "login %s, %s\n", username, password);//
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("Connection to the server failed...\n");
            exit(0);
        }
        send(client_socket, buffer, sizeof(buffer), 0);//
        memset(buffer, 0, sizeof(buffer));//
        recv(client_socket, buffer, sizeof(buffer), 0);
        closesocket(client_socket);
        cJSON* eb = cJSON_Parse(buffer);
        strcpy(token, cJSON_GetObjectItem(eb , "content")->valuestring);// copying token to an array

        system("@cls||clear");//erasing the previous messages

        if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "AuthToken") == 0)
            printf("Welcome!\n");
        else{
            puts("Username or password is wrong!");
            chat(client_socket);
        }
    }

    while(1){//the big while which contains everything after login


        printf("What the hell do you want to do?\n1: Create channel\n2: Join channel\n3: Logout\n");
        scanf("%d", &demand);

        if(demand == 1 || demand == 2){
            puts("Enter channel's name:");
            scanf("%s", &channel_name);
        }

        system("@cls||clear");//erasing the previous messages

        memset(buffer, 0, sizeof(buffer));//
        while(demand == 1){//making channel

            sprintf(buffer, "create channel %s, %s\n", channel_name, token);
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                printf("Connection to the server failed...\n");
                exit(0);
            }
            send(client_socket, buffer, sizeof(buffer), 0);//sending channel and token to server
            memset(buffer, 0, sizeof(buffer));
            recv(client_socket, buffer, sizeof(buffer), 0);
            closesocket(client_socket);
            cJSON* eb = cJSON_Parse(buffer);

            if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Successful") == 0)
                printf("Channel created!\n");
            else{
                puts("Channel not created... Probably a channel with this name exists, try again.");
            }

            memset(buffer, 0, sizeof(buffer));
            printf("What the hell do you want to do?\n1: Create channel\n2: Join channel\n3: Logout\n");
            scanf("%d", &demand);
            if(demand == 1 || demand == 2){
                puts("Enter channel's name:");
                scanf("%s", &channel_name);
            }
        }


        if(demand == 2){//joining channel
            sprintf(buffer, "join %s\n", token);
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {//loooooooook at this part and fix it
                printf("Connection to the server failed...\n");
                exit(0);
            }
            send(client_socket, buffer, sizeof(buffer), 0);//sending channel and token to server
            memset(buffer, 0, sizeof(buffer));//
            //printf("channel_name = %s\n", channel_name);
            system("@cls||clear");//erasing the previous messages
            sprintf(buffer, "join channel %s, %s\n", channel_name, token);
            //printf("buffer = %s\n", buffer);
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                printf("Connection to the server failed...\n");
                exit(0);
            }
            send(client_socket, buffer, sizeof(buffer), 0);//sending channel and token to server
            memset(buffer, 0, sizeof(buffer));//
            recv(client_socket, buffer, sizeof(buffer), 0);
            closesocket(client_socket);
            cJSON* eb = cJSON_Parse(buffer);
            if(strcmp(cJSON_GetObjectItem(eb , "content")->valuestring, "Channel not found.") == 0){
                puts("Channel not found... Try again.");
                continue;
            }


            puts("You joined the channel!");
            while(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Successful") == 0){
                puts("What do you want to do?\n1: Send a message\n2: Refresh\n3: Viewing channel's members\n4: Leave channel\n5: Search for members\n6: Search for words\n");
                scanf("%d", &demand_channel);
                system("@cls||clear");//erasing the previous messages

                if(demand_channel == 1){//sending message
                    puts("Enter message:");
                    gets(message);
                    memset(message, 0, sizeof(message));//
                    gets(message);
                    system("@cls||clear");//erasing the previous messages
                    memset(send_recv, 0, sizeof(send_recv));//
                    sprintf(send_recv, "send %s, %s\n", message, token);
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                        printf("Connection to the server failed...\n");
                        exit(0);
                    }
                    send(client_socket, send_recv, sizeof(send_recv), 0);//sending message and token to server
                    memset(buffer, 0, sizeof(buffer));//
                    recv(client_socket, buffer, sizeof(buffer), 0);
                    closesocket(client_socket);
                    cJSON* eb = cJSON_Parse(buffer);
                    if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Error") == 0){
                        puts("There is a problem in sending the message. Try again.");
                    }else{
                        printf("You:%s\n", message);
                        memset(message, 0, sizeof(message));
                    }
                }


                if(demand_channel == 2){//refresh
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if(connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                    printf("Connection to the server failed...\n");
                    exit(0);
                    }

                    memset(buffer, 0, sizeof(buffer));//
                    sprintf(buffer, "refresh %s\n", token);
                    send(client_socket, buffer, sizeof(buffer), 0);//sending message and token to server
                    memset(buffer, 0, sizeof(buffer));//
                    memset(send_recv, 0, sizeof(send_recv));//
                    recv(client_socket, send_recv, sizeof(send_recv), 0);
                    closesocket(client_socket);
                    cJSON* eb = cJSON_Parse(send_recv);
                    printf("%s\n", cJSON_Print(cJSON_GetObjectItem(eb , "content")));
                }


                if(demand_channel == 3){//showing the list of members
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if(connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                    printf("Connection to the server failed...\n");
                    exit(0);
                    }

                    memset(buffer, 0, sizeof(buffer));//
                    sprintf(buffer, "channel members %s\n", token);
                    send(client_socket, buffer, sizeof(buffer), 0);//sending message and token to server
                    memset(buffer, 0, sizeof(buffer));//
                    memset(send_recv, 0, sizeof(send_recv));
                    recv(client_socket, send_recv, sizeof(send_recv), 0);
                    closesocket(client_socket);
                    cJSON* eb = cJSON_Parse(send_recv);

                    system("@cls||clear");//erasing the previous messages

                    printf("%s\n", cJSON_Print(cJSON_GetObjectItem(eb , "content")));
                    memset(send_recv, 0, sizeof(send_recv));
                }


                if(demand_channel == 4){//leave channel
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if(connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                    printf("Connection to the server failed...\n");
                    exit(0);
                    }

                    memset(buffer, 0, sizeof(buffer));//
                    sprintf(buffer, "leave %s\n", token);
                    send(client_socket, buffer, sizeof(buffer), 0);//sending message and token to server
                    memset(buffer, 0, sizeof(buffer));//
                    recv(client_socket, buffer, sizeof(buffer), 0);
                    closesocket(client_socket);

                    system("@cls||clear");//erasing the previous messages

                    if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Successful") == 0){
                        puts("You left the channel.");
                        break;
                    }else
                        puts("Sorry, there was a problem, order couldn't be executed.");

                }

                if(demand_channel == 5){//searching for members in channel
                    system("@cls||clear");//erasing the previous messages

                    puts("Enter member's name:");
                    char search_name[MAX];
                    scanf("%s", search_name);
                    memset(buffer, 0, sizeof(buffer));//
                    sprintf(buffer, "search member %s %s", search_name, token);
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                        printf("Connection to the server failed...\n");
                        exit(0);
                    }
                    send(client_socket, buffer, sizeof(buffer), 0);
                    //printf("buffer = %s\n", buffer);
                    memset(buffer, 0, sizeof(buffer));//
                    recv(client_socket, buffer, sizeof(buffer), 0);
                    closesocket(client_socket);

                    system("@cls||clear");//erasing the previous messages

                    if(strcmp(buffer, "true") == 0)
                        puts("A user with this name exists in this channel!");
                    if(strcmp(buffer, "false") == 0)
                        puts("There is no user with this name in this channel...");

                }

                if(demand_channel == 6){//search for words
                    system("@cls||clear");//erasing the previous messages

                    puts("Enter word:");
                    char search_word[MAX];
                    scanf("%s", search_word);
                    memset(buffer, 0, sizeof(buffer));//
                    sprintf(buffer, "search word %s %s", search_word, token);
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                        printf("Connection to the server failed...\n");
                        exit(0);
                    }
                    send(client_socket, buffer, sizeof(buffer), 0);
                    //printf("buffer = %s\n", buffer);
                    memset(buffer, 0, sizeof(buffer));//

                    system("@cls||clear");//erasing the previous messages

                    int k = 0;
                    for( ; ; k++){

                        if(strcmp(buffer, "end_of_search") == 0)//in case search had no result
                            break;

                        recv(client_socket, buffer, sizeof(buffer), 0);
                        closesocket(client_socket);
                        printf("%s\n", buffer);
                        memset(buffer, 0, sizeof(buffer));
                    }
                    if(k == 0)
                        puts("Search had no result...");

                }

                memset(buffer, 0, sizeof(buffer));
            }
        }

        while(demand == 3){//logout
            sprintf(buffer, "logout %s\n", token);
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                printf("Connection to the server failed...\n");
                exit(0);
            }
            send(client_socket, buffer, sizeof(buffer), 0);//sending channel and token to server
            memset(buffer, 0, sizeof(buffer));//
            recv(client_socket, buffer, sizeof(buffer), 0);//{"type":"Successful","content":""}
            closesocket(client_socket);

            system("@cls||clear");//erasing the previous messages

            cJSON* eb = cJSON_Parse(buffer);
            if(strcmp(cJSON_GetObjectItem(eb , "type")->valuestring, "Successful") == 0){
                puts("Have a nice day!");
                chat(client_socket);//restart the process
            }
            memset(buffer, 0, sizeof(buffer));//
        }


    }//the big while which contains everything after login
}

int main()
{
    //coloring the background:
    char sysmes[] = "color BF";
    sysmes[6] = '5';
    sysmes[7] = 'e';
    system(sysmes);


	int client_socket, server_socket;
	struct sockaddr_in cli;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

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
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);


	// Function for chat
	chat(client_socket);

	// Close the socket
	closesocket(client_socket);
}
