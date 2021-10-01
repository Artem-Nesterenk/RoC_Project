/*file curl.c*/



void getImg(char* fileSpec);

void* img_Queue(void*);
void img_Queue_Handler(int);
void get_Img_Handler(char* ip, int cfd);
